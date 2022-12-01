//
// Created by andyroiiid on 11/30/2022.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <foundation/PxVec3.h>

#include <haru/render/MeshBase.h>

using EntityProperties = std::map<std::string, std::string>;

struct EntityBrushes {
    std::map<std::string, std::vector<VertexBase>> TextureToVertices;
    std::vector<std::vector<physx::PxVec3>> Colliders;
};

struct EntityDefinition {
    EntityProperties Properties;
    EntityBrushes Brushes;

    [[nodiscard]] const std::string &GetProperty(const std::string &key, const std::string &fallback = "") const;
};
