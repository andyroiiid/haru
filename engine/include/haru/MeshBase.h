//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "haru/Mesh.h"

struct VertexBase {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;

    static void SetupVertexArray(const GLuint vao) {
        SetupVertexArrayFloatsAttrib(vao, 0, 0, 3, offsetof(VertexBase, Position));
        SetupVertexArrayFloatsAttrib(vao, 1, 0, 3, offsetof(VertexBase, Normal));
        SetupVertexArrayFloatsAttrib(vao, 2, 0, 2, offsetof(VertexBase, TexCoord));
    }
};

using MeshBase = Mesh<VertexBase>;