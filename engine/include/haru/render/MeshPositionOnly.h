//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <glm/vec2.hpp>

#include "haru/render/Mesh.h"

struct VertexPositionOnly {
    glm::vec3 Position;

    static void SetupVertexArray(const GLuint vao) {
        SetupVertexArrayFloatsAttrib(vao, 0, 0, 3, offsetof(VertexPositionOnly, Position));
    }
};

using MeshPositionOnly = Mesh<VertexPositionOnly>;
