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
}

PhysicsSystem::~PhysicsSystem() {
    PX_RELEASE(m_physics)
    PX_RELEASE(m_pvd)
    PX_RELEASE(m_pvdTransport)
    PX_RELEASE(m_foundation)
}
