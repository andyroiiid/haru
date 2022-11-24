//
// Created by andyroiiid on 11/21/2022.
//

#include "haru/physics/PhysicsSystem.h"

#include <PxPhysicsAPI.h>

#include "haru/core/Debug.h"

static physx::PxDefaultAllocator s_Allocator;
static physx::PxDefaultErrorCallback s_ErrorCallback;

PhysicsSystem::PhysicsSystem() {
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, s_Allocator, s_ErrorCallback);

    m_pvdTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);

    m_pvd = PxCreatePvd(*m_foundation);

    if (!m_pvd->connect(*m_pvdTransport, physx::PxPvdInstrumentationFlag::eALL)) {
        DebugLog("Failed to start PVD");
    }

    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, physx::PxTolerancesScale(), true, m_pvd);

    physx::PxCookingParams cookingParams(m_physics->getTolerancesScale());
    m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, cookingParams);
}

PhysicsSystem::~PhysicsSystem() {
    PX_RELEASE(m_cooking);
    PX_RELEASE(m_physics)
    PX_RELEASE(m_pvd)
    PX_RELEASE(m_pvdTransport)
    PX_RELEASE(m_foundation)
}

physx::PxConvexMesh *PhysicsSystem::CreateConvexMesh(physx::PxU32 count, const physx::PxVec3 *vertices, physx::PxU16 vertexLimit) {
    physx::PxConvexMeshDesc desc;
    desc.points.count = count;
    desc.points.stride = sizeof(physx::PxVec3);
    desc.points.data = vertices;
    desc.flags =
            physx::PxConvexFlag::eCOMPUTE_CONVEX |
            physx::PxConvexFlag::eDISABLE_MESH_VALIDATION |
            physx::PxConvexFlag::eFAST_INERTIA_COMPUTATION;
    desc.vertexLimit = vertexLimit;

    physx::PxDefaultMemoryOutputStream buffer;
    if (!m_cooking->cookConvexMesh(desc, buffer)) {
        DebugLog("Failed to create convex mesh!");
        return nullptr;
    }

    physx::PxDefaultMemoryInputData input(buffer.getData(), buffer.getSize());
    return m_physics->createConvexMesh(input);
}
