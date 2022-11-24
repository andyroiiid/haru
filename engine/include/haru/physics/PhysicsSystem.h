//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include "haru/core/MoveOnly.h"

#include <foundation/Px.h>

namespace physx {
    class PxFoundation;

    class PxPvdTransport;

    class PxPvd;

    class PxPhysics;

    class PxCooking;

    class PxConvexMesh;
}

class PhysicsSystem {
public:
    NO_MOVE_OR_COPY(PhysicsSystem)

    PhysicsSystem();

    ~PhysicsSystem();

    physx::PxConvexMesh *CreateConvexMesh(
            physx::PxU32 count,
            const physx::PxVec3 *vertices,
            // The number of vertices and faces of a convex mesh in PhysX is limited to 255.
            physx::PxU16 vertexLimit = 255
    );

private:
    friend class PhysicsScene;

    physx::PxFoundation *m_foundation = nullptr;
    physx::PxPvdTransport *m_pvdTransport = nullptr;
    physx::PxPvd *m_pvd = nullptr;
    physx::PxPhysics *m_physics = nullptr;
    physx::PxCooking *m_cooking = nullptr;
};
