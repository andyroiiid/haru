//
// Created by andyroiiid on 11/30/2022.
//

#pragma once

#include <map>
#include <string>
#include <vector>
#include <foundation/PxVec3.h>
#include <glm/vec3.hpp>

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

    [[nodiscard]] bool GetPropertyDirection(const std::string &key, glm::vec3 &value) const {
        if (!GetPropertyVec3(key, value)) return false;
        value = {value.x, value.z, -value.y}; // convert from quake direction to haru direction
        return true;
    }

    [[nodiscard]] bool GetPropertyPosition(const std::string &key, glm::vec3 &value) const {
        if (!GetPropertyVec3(key, value)) return false;
        value = glm::vec3{value.x, value.z, -value.y} / 32.0f; // convert from quake direction to haru direction
        return true;
    }

private:
    [[nodiscard]] bool GetPropertyVec3(const std::string &key, glm::vec3 &value) const;
};
