//
// Created by Andrew Huang on 11/22/2022.
//

#pragma once

#include <geometry/PxGeometry.h>
#include <PxQueryReport.h>

#include "haru/core/MoveOnly.h"
#include "haru/physics/PhysicsSystem.h"

namespace physx {
    class PxPhysics;

    class PxMaterial;

    class PxScene;

    class PxRigidStatic;

    class PxRigidDynamic;
}

class PhysicsScene {
public:
    NO_MOVE_OR_COPY(PhysicsScene)

    explicit PhysicsScene(PhysicsSystem *physicsSystem);

    ~PhysicsScene();

    void Update(float deltaTime, float timeScale);

    physx::PxRigidStatic *CreateStatic(const physx::PxTransform &transform, const physx::PxGeometry &geometry);

    physx::PxRigidDynamic *CreateDynamic(const physx::PxTransform &transform, const physx::PxGeometry &geometry);

    [[nodiscard]] physx::PxRaycastBuffer Raycast(
            const physx::PxVec3 &origin,
            const physx::PxVec3 &unitDir,
            float distance
    ) const;

private:
    PhysicsSystem *m_physicsSystem;
    physx::PxPhysics *m_physics = nullptr;
    physx::PxMaterial *m_defaultMaterial = nullptr;

    physx::PxScene *m_scene = nullptr;
    float m_timeSinceLastTick = 0.0f;
};
