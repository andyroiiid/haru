//
// Created by andyroiiid on 11/21/2022.
//

#include "haru/render/DeferredShaders.h"

DeferredShaderShadowPass::DeferredShaderShadowPass() : Shader(Shader::FromFile(
        "shaders/shared.glsl",
        "shaders/shadow_pass.vert",
        "shaders/shadow_pass.geom",
        "shaders/shadow_pass.frag"
)) {
    m_modelLocation = GetUniformLocation("uModel");
}

void DeferredShaderShadowPass::SetModel(const glm::mat4 &model) {
    SetUniform(m_modelLocation, model);
}

DeferredShaderBase::DeferredShaderBase() : Shader(FromFile(
        "shaders/shared.glsl",
        "shaders/base.vert",
        "shaders/base.frag"
)) {
    m_modelLocation = GetUniformLocation("uModel");
}

void DeferredShaderBase::SetModel(const glm::mat4 &model) {
    SetUniform(m_modelLocation, model);
}

DeferredShaderGPass::DeferredShaderGPass() : Shader(FromFile(
        "shaders/shared.glsl",
        "shaders/g_pass.vert",
        "shaders/g_pass.frag"
)) {
}

DeferredShaderLines::DeferredShaderLines() : Shader(FromFile(
        "shaders/shared.glsl",
        "shaders/lines.vert",
        "shaders/lines.frag"
)) {
    m_colorLocation = GetUniformLocation("uColor");
}

void DeferredShaderLines::SetColor(const glm::vec4 &color) {
    SetUniform(m_colorLocation, color);
}

DeferredShaderSkybox::DeferredShaderSkybox() : Shader(FromFile(
        "shaders/shared.glsl",
        "shaders/skybox.vert",
        "shaders/skybox.frag"
)) {
}
