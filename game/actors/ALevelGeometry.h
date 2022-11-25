//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include <glm/vec3.hpp>

namespace physx {
    class PxConvexMesh;

    class PxRigidStatic;
}

#include "Actor.h"

class ALevelGeometry : public Actor {
public:
    DEFINE_ACTOR_CLASS(ALevelGeometry)

    explicit ALevelGeometry(const std::string &levelName);

    ~ALevelGeometry() override;

    void Draw(Renderer &renderer) override;

private:
    MeshBase m_mesh;
    std::vector<physx::PxConvexMesh *> m_brushColliders;
    std::vector<physx::PxRigidStatic *> m_brushRigidbodies;

    glm::vec3 m_geomMin{};
    glm::vec3 m_geomMax{};
};
