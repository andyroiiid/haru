//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include "haru/core/MoveOnly.h"

namespace physx {
    class PxFoundation;

    class PxPvdTransport;

    class PxPvd;

    class PxPhysics;

    class PxDefaultCpuDispatcher;

    class PxMaterial;
}

class PhysicsSystem {
public:
    NO_MOVE_OR_COPY(PhysicsSystem)

    PhysicsSystem();

    ~PhysicsSystem();

private:
    friend class PhysicsScene;

    physx::PxFoundation *m_foundation = nullptr;
    physx::PxPvdTransport *m_pvdTransport = nullptr;
    physx::PxPvd *m_pvd = nullptr;
    physx::PxPhysics *m_physics = nullptr;
    physx::PxDefaultCpuDispatcher *m_dispatcher = nullptr;
    physx::PxMaterial *m_defaultMaterial = nullptr;
};
