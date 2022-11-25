//
// Created by andyroiiid on 11/20/2022.
//

#include "haru/render/DeferredRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

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

void DeferredRenderer::SetDirectionalLight(const glm::vec3 &lightDirection, const float intensity) {
    m_lightGlobals->DirectionalLight = lightDirection;
    m_lightGlobals->DirectionalLightIntensity = intensity;

    m_shadowMatrixCalculator.SetLightDirection(lightDirection);
}

void DeferredRenderer::SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) {
    static constexpr float SHADOW_SAFE_DISTANCE = 4.0f;
    m_shadowMatrixCalculator.SetWorldBounds(min - SHADOW_SAFE_DISTANCE, max + SHADOW_SAFE_DISTANCE);
}

void DeferredRenderer::BeginDraw() {
    m_pendingPointLightData.clear();
    m_pendingBaseDrawCalls.clear();
    m_pendingLinesDrawCalls.clear();

    m_shaderGlobals.Map();
    m_lightGlobals.Map();
}

void DeferredRenderer::EndDraw() {
    FlushUniformBuffers();
    DrawToShadowMap();
    DrawToGBuffers();
    DrawForwardPass();
}

void DeferredRenderer::DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, const float linear, const float quadratic) {
    m_pendingPointLightData.push_back({position, linear, color, quadratic});
}

void DeferredRenderer::DrawLines(const MeshPositionOnly &lines, const glm::vec4 &color) {
    m_pendingLinesDrawCalls.push_back({lines, color});
}

void DeferredRenderer::DrawMesh(const MeshBase &mesh, const glm::mat4 &model) {
    m_pendingBaseDrawCalls.push_back({mesh, model});
}

void DeferredRenderer::FlushUniformBuffers() {
    m_shaderGlobals.Unmap();

    constexpr float shadowNear = 0.01f;
    constexpr float shadowFar = 32.0f;
    constexpr glm::vec3 csmSplits{4.0f, 8.0f, 16.0f};
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
    m_gBuffers.Bind();
    glViewport(0, 0, m_gBuffers.Size().x, m_gBuffers.Size().y);

    m_gBuffers.Clear();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    m_baseShader.Use();
    for (const auto &baseDrawCall: m_pendingBaseDrawCalls) {
        m_baseShader.SetModel(baseDrawCall.Model);
        baseDrawCall.Mesh.BindAndDraw();
    }

    glBindVertexArray(0);
    glBindTextureUnit(0, 0);
    glUseProgram(0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
}

void DeferredRenderer::DrawForwardPass() {
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
    for (const auto &linesDrawCall: m_pendingLinesDrawCalls) {
        m_linesShader.SetColor(linesDrawCall.Color);
        linesDrawCall.Mesh.BindAndDraw();
    }

    glDepthFunc(GL_LEQUAL);
    m_skyboxShader.Use();
    m_skyboxCube.BindAndDraw();
    glDepthFunc(GL_LESS);

    glDisable(GL_DEPTH_TEST);
}
