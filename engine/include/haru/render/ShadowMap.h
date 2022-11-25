//
// Created by andyroiiid on 11/25/2022.
//

#pragma once

#include <glad/gl.h>

#include "haru/core/MoveOnly.h"

class ShadowMap {
public:
    MOVE_ONLY(ShadowMap)

    ShadowMap() = default;

    explicit ShadowMap(int size);

    ~ShadowMap();

    [[nodiscard]] int Size() const { return m_size; }

    void Bind();

    void BindDepthTexture(GLuint unit);

private:
    MoveOnly<int> m_size;

    MoveOnly<GLuint> m_fbo;
    MoveOnly<GLuint> m_depthTexture;
};
