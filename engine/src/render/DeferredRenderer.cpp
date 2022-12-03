//
// Created by andyroiiid on 11/20/2022.
//

#include "haru/render/DeferredRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <tracy/Tracy.hpp>

DeferredRenderer::DeferredRenderer() {
    m_shaderGlobals.Bind(0);
    m_lightGlobals.Bind(1);

    m_fullscreenQuad = MeshPositionOnly(
            {
                    {{-1.0f, -1.0f, 0.0f}},
                    {{1.0f,  -1.0f, 0.0f}},
                    {{-1.0f, 1.0f,  0.0f}},
                    {{1.0f,  1.0f,  0.0f}},
            },
            GL_TRIANGLE_STRIP
    );

    // http://www.cs.umd.edu/gvil/papers/av_ts.pdf
    m_skyboxCube = MeshPositionOnly(
            {
                    {{-1.0f, 1.0f,  1.0f}},
                    {{1.0f,  1.0f,  1.0f}},
                    {{-1.0f, -1.0f, 1.0f}},
                    {{1.0f,  -1.0f, 1.0f}},
                    {{1.0f,  -1.0f, -1.0f}},
                    {{1.0f,  1.0f,  1.0f}},
                    {{1.0f,  1.0f,  -1.0f}},
                    {{-1.0f, 1.0f,  1.0f}},
                    {{-1.0f, 1.0f,  -1.0f}},
                    {{-1.0f, -1.0f, 1.0f}},
                    {{-1.0f, -1.0f, -1.0f}},
                    {{1.0f,  -1.0f, -1.0f}},
                    {{-1.0f, 1.0f,  -1.0f}},
                    {{1.0f,  1.0f,  -1.0}},
            },
            GL_TRIANGLE_STRIP
    );

    static constexpr int DEFAULT_DIFFUSE_TEXTURE_SIZE = 4;
    static constexpr unsigned char DEFAULT_DIFFUSE_TEXTURE_DATA[DEFAULT_DIFFUSE_TEXTURE_SIZE * DEFAULT_DIFFUSE_TEXTURE_SIZE * 4] = {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    };
    m_defaultDiffuseTexture = Texture({DEFAULT_DIFFUSE_TEXTURE_SIZE, DEFAULT_DIFFUSE_TEXTURE_SIZE}, DEFAULT_DIFFUSE_TEXTURE_DATA);
}

DeferredRenderer::~DeferredRenderer() {
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, 0);
}

void DeferredRenderer::OnResize(const glm::ivec2 &size) {
    m_gBuffers = Framebuffer(
            size,
            {
                    GL_RGBA32F, // WorldPosition
                    GL_RGBA32F, // WorldNormal
                    GL_RGBA8 // Color
            }
    );
}

void DeferredRenderer::SetCameraInfo(const glm::mat4 &view, float fov, float near, float far) {
    m_shaderGlobals->View = view;

    const glm::vec2 screenSize = GetSize();
    const float aspectRatio = screenSize.x / screenSize.y;
    m_shaderGlobals->Projection = glm::perspective(glm::radians(fov), aspectRatio, near, far);

    m_shadowMatrixCalculator.SetCameraInfo(view, fov, aspectRatio);
}

void DeferredRenderer::SetWorldLight(const glm::vec3 &lightDirection, const glm::vec3 &lightColor, const glm::vec3 &ambientColor) {
    m_lightGlobals->DirectionalLight = lightDirection;
    m_lightGlobals->DirectionalColor = lightColor;
    m_lightGlobals->AmbientColor = ambientColor;

    m_shadowMatrixCalculator.SetLightDirection(lightDirection);
}

void DeferredRenderer::SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) {
    static constexpr float SHADOW_SAFE_DISTANCE = 4.0f;
    m_shadowMatrixCalculator.SetWorldBounds(min - SHADOW_SAFE_DISTANCE, max + SHADOW_SAFE_DISTANCE);
}

void DeferredRenderer::BeginDraw() {
    ZoneScoped;

    m_pendingPointLightData.clear();
    m_pendingLines.clear();
    m_pendingBaseDrawCalls.clear();

    m_shaderGlobals.Map();
    m_lightGlobals.Map();
}

void DeferredRenderer::EndDraw() {
    ZoneScoped;

    FlushUniformBuffers();
    DrawToShadowMap();
    DrawToGBuffers();
    DrawForwardPass();
    glFinish();
}

void DeferredRenderer::DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, const float linear, const float quadratic) {
    m_pendingPointLightData.push_back({position, linear, color, quadratic});
}

void DeferredRenderer::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &color) {
    m_pendingLines.push_back({p0, color});
    m_pendingLines.push_back({p1, color});
}

void DeferredRenderer::DrawMesh(const MeshBase &mesh, const glm::mat4 &model, const Material *material) {
    m_pendingBaseDrawCalls.push_back({mesh, model, material ? *material : m_defaultMaterial});
}

void DeferredRenderer::FlushUniformBuffers() {
    ZoneScoped;

    m_shaderGlobals.Unmap();

    constexpr float shadowNear = 0.01f;
    constexpr float shadowFar = 64.0f;
    constexpr glm::vec3 csmSplits{8.0f, 16.0f, 32.0f};
    m_lightGlobals->CascadeShadowMapSplits = csmSplits;
    m_lightGlobals->ShadowMatrices[0] = m_shadowMatrixCalculator.CalcShadowMatrix(shadowNear, csmSplits[0]);
    m_lightGlobals->ShadowMatrices[1] = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[0], csmSplits[1]);
    m_lightGlobals->ShadowMatrices[2] = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[1], csmSplits[2]);
    m_lightGlobals->ShadowMatrices[3] = m_shadowMatrixCalculator.CalcShadowMatrix(csmSplits[2], shadowFar);

    const size_t numPointLights = m_pendingPointLightData.size();
    for (size_t i = 0; i < 32; i++) {
        m_lightGlobals->PointLightData[i] = i < numPointLights ? m_pendingPointLightData[i] : PointLightData{};
    }

    m_lightGlobals.Unmap();
}

void DeferredRenderer::DrawToShadowMap() {
    ZoneScoped;

    m_shadowMap.Bind();
    glViewport(0, 0, m_shadowMap.Size(), m_shadowMap.Size());

    glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

    glEnable(GL_DEPTH_TEST);

    m_shadowShader.Use();
    for (const auto &baseDrawCall: m_pendingBaseDrawCalls) {
        m_shadowShader.SetModel(baseDrawCall.Model);
        baseDrawCall.Mesh.BindAndDraw();
    }

    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_DEPTH_TEST);
}

void DeferredRenderer::DrawToGBuffers() {
    ZoneScoped;

    m_gBuffers.Bind();
    glViewport(0, 0, m_gBuffers.Size().x, m_gBuffers.Size().y);

    m_gBuffers.Clear();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_baseShader.Use();
    for (const auto &baseDrawCall: m_pendingBaseDrawCalls) {
        m_baseShader.SetModel(baseDrawCall.Model);
        baseDrawCall.material.Diffuse->Bind(0);
        baseDrawCall.Mesh.BindAndDraw();
    }

    glBindVertexArray(0);
    glBindTextureUnit(0, 0);
    glUseProgram(0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

void DeferredRenderer::DrawForwardPass() {
    ZoneScoped;

    Framebuffer::BindDefault();
    glViewport(0, 0, GetSize().x, GetSize().y);

    constexpr GLfloat clearColor[4]{0.0f, 0.0f, 0.0f, 1.0f};
    glClearBufferfv(GL_COLOR, 0, clearColor);
    glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

    // draw to g-buffers
    m_gPassShader.Use();
    m_gBuffers.BindAllTextures();
    m_shadowMap.BindDepthTexture(m_gBuffers.GetTextureCount());
    m_fullscreenQuad.BindAndDraw();
    glBindVertexArray(0);
    m_gBuffers.UnbindAllTextures();
    glUseProgram(0);

    // blit depth & stencil from g-buffers to screen
    m_gBuffers.BlitDepthStencilToScreen(GetSize());

    glEnable(GL_DEPTH_TEST);

    // draw lines
    m_linesShader.Use();
    m_linesMesh.UpdateData(m_pendingLines);
    m_linesMesh.BindAndDraw();

    glDepthFunc(GL_LEQUAL);
    m_skyboxShader.Use();
    m_skyboxCube.BindAndDraw();
    glDepthFunc(GL_LESS);

    glDisable(GL_DEPTH_TEST);
}
