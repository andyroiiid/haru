//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include "haru/core/MoveOnly.h"

#include <geometry/PxGeometry.h>
#include <PxQueryReport.h>

namespace physx {
    class PxFoundation;

    class PxPvdTransport;

    class PxPvd;

    class PxPhysics;

    class PxDefaultCpuDispatcher;

    class PxMaterial;

    class PxScene;

    class PxRigidStatic;

    class PxRigidDynamic;
}

class PhysicsSystem {
public:
    MOVE_ONLY(PhysicsSystem)

    PhysicsSystem();

    ~PhysicsSystem();

    void Update(float deltaTime);

    physx::PxRigidStatic *CreateStatic(const physx::PxTransform &transform, const physx::PxGeometry &geometry);

    physx::PxRigidDynamic *CreateDynamic(const physx::PxTransform &transform, const physx::PxGeometry &geometry);

    [[nodiscard]] physx::PxRaycastBuffer Raycast(
            const physx::PxVec3 &origin,
            const physx::PxVec3 &unitDir,
            float distance
    ) const;

private:
    physx::PxFoundation *m_foundation = nullptr;
    physx::PxPvdTransport *m_pvdTransport = nullptr;
    physx::PxPvd *m_pvd = nullptr;
    physx::PxPhysics *m_physics = nullptr;
    physx::PxDefaultCpuDispatcher *m_dispatcher = nullptr;

    physx::PxMaterial *m_defaultMaterial = nullptr;
    physx::PxScene *m_scene = nullptr;

    float m_timeSinceLastTick = 0.0f;
};
