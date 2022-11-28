//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include "haru/render/Shader.h"

class DeferredShadersPrelude {
public:
    NO_MOVE_OR_COPY(DeferredShadersPrelude)

    DeferredShadersPrelude();

    ~DeferredShadersPrelude();
};

class DeferredShaderShadowPass : public Shader {
public:
    MOVE_ONLY(DeferredShaderShadowPass)

    DeferredShaderShadowPass();

    void SetModel(const glm::mat4 &model);

private:
    GLint m_modelLocation = -1;
};

class DeferredShaderBase : public Shader {
public:
    MOVE_ONLY(DeferredShaderBase)

    DeferredShaderBase();

    void SetModel(const glm::mat4 &model);

private:
    GLint m_modelLocation = -1;
};

class DeferredShaderGPass : public Shader {
public:
    MOVE_ONLY(DeferredShaderGPass)

    DeferredShaderGPass();
};

class DeferredShaderLines : public Shader {
public:
    MOVE_ONLY(DeferredShaderLines)

    DeferredShaderLines();

    void SetColor(const glm::vec4 &color);

private:
    GLint m_colorLocation = -1;
};

class DeferredShaderSkybox : public Shader {
public:
    MOVE_ONLY(DeferredShaderSkybox)

    DeferredShaderSkybox();
};
