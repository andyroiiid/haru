//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

namespace physx {
    class PxConvexMesh;

    class PxRigidStatic;
}

#include "Actor.h"

class ALevelGeom : public Actor {
public:
    DEFINE_ACTOR_CLASS(ALevelGeom)

    explicit ALevelGeom(const std::string &levelName);

    ~ALevelGeom() override;

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    MeshBase m_mesh;
    std::vector<physx::PxConvexMesh *> m_brushColliders;
    std::vector<physx::PxRigidStatic *> m_brushRigidbodies;
};
