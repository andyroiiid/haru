//
// Created by andyroiiid on 11/20/2022.
//

#include "haru/render/DeferredRenderer.h"

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

void DeferredRenderer::SetCameraMatrices(const glm::mat4 &view, const glm::mat4 &projection) {
    m_shaderGlobals->View = view;
    m_shaderGlobals->Projection = projection;
}

void DeferredRenderer::SetDirectionalLight(const glm::vec3 &lightDirection, const float intensity) {
    m_lightGlobals->DirectionalLight = lightDirection;
    m_lightGlobals->DirectionalLightIntensity = intensity;
}

void DeferredRenderer::BeginDraw() {
    m_pendingPointLightData.clear();
    m_pendingBaseDrawCalls.clear();
    m_pendingLinesDrawCalls.clear();
}

void DeferredRenderer::EndDraw() {
    FlushUniformBuffers();
    DrawToGBuffers();
    DrawForwardPass();
}

void DeferredRenderer::DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, const float linear,
                                      const float quadratic) {
    m_pendingPointLightData.push_back({position, linear, color, quadratic});
}

void DeferredRenderer::DrawLines(const MeshPositionOnly &lines, const glm::vec4 &color) {
    m_pendingLinesDrawCalls.push_back({lines, color});
}

void DeferredRenderer::DrawMesh(const MeshBase &mesh, const glm::mat4 &model) {
    m_pendingBaseDrawCalls.push_back({mesh, model});
}

void DeferredRenderer::FlushUniformBuffers() {
    m_shaderGlobals.Flush();

    const size_t numPointLights = m_pendingPointLightData.size();
    for (size_t i = 0; i < 32; i++) {
        m_lightGlobals->PointLightData[i] = i < numPointLights ? m_pendingPointLightData[i] : PointLightData{};
    }

    m_lightGlobals.Flush();
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

    glDisable(GL_DEPTH_TEST);
}
