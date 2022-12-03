//
// Created by andyroiiid on 12/2/2022.
//

#pragma once

#include <glm/vec3.hpp>

#include "haru/render/Mesh.h"

struct VertexPositionColor {
    glm::vec3 Position;
    glm::vec3 Color;

    static void SetupVertexArray(const GLuint vao) {
        SetupVertexArrayFloatsAttrib(vao, 0, 0, 3, offsetof(VertexPositionColor, Position));
        SetupVertexArrayFloatsAttrib(vao, 1, 0, 3, offsetof(VertexPositionColor, Color));
    }
};

using MeshPositionColor = Mesh<VertexPositionColor>;
