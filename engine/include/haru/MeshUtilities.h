//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#pragma once

#include "haru/MeshBase.h"

std::vector<VertexBase> CreateBox(const glm::vec3 &min, const glm::vec3 &max);

static std::vector<VertexBase> CreateBox(const glm::vec3 &halfSize) {
    return CreateBox(-halfSize, halfSize);
}