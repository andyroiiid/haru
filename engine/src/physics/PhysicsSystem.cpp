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

    m_dispatcher = physx::PxDefaultCpuDispatcherCreate(2);

    m_defaultMaterial = m_physics->createMaterial(0.8f, 0.8f, 0.25f);

    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = m_dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_scene = m_physics->createScene(sceneDesc);

    if (physx::PxPvdSceneClient *pvdClient = m_scene->getScenePvdClient()) {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

PhysicsSystem::~PhysicsSystem() {
    PX_RELEASE(m_scene)
    PX_RELEASE(m_defaultMaterial)

    PX_RELEASE(m_dispatcher)
    PX_RELEASE(m_physics)
    PX_RELEASE(m_pvd)
    PX_RELEASE(m_pvdTransport)
    PX_RELEASE(m_foundation)
}

void PhysicsSystem::Update(const float deltaTime, const float timeScale) {
    m_timeSinceLastTick += deltaTime * timeScale;

    static constexpr float FIXED_TIMESTEP = 0.02f;
    const float scaledTimestep = FIXED_TIMESTEP * timeScale;

    if (m_timeSinceLastTick >= scaledTimestep) {
        m_scene->simulate(scaledTimestep);
        m_scene->fetchResults(true);
        m_timeSinceLastTick -= scaledTimestep;
    }
}

physx::PxRigidStatic *PhysicsSystem::CreateStatic(
        const physx::PxTransform &transform,
        const physx::PxGeometry &geometry
) {
    physx::PxRigidStatic *actor = PxCreateStatic(*m_physics, transform, geometry, *m_defaultMaterial);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRigidDynamic *PhysicsSystem::CreateDynamic(
        const physx::PxTransform &transform,
        const physx::PxGeometry &geometry
) {
    physx::PxRigidDynamic *actor = PxCreateDynamic(*m_physics, transform, geometry, *m_defaultMaterial, 1.0f);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRaycastBuffer PhysicsSystem::Raycast(
        const physx::PxVec3 &origin,
        const physx::PxVec3 &unitDir,
        const float distance
) const {
    physx::PxRaycastBuffer hitCall;
    m_scene->raycast(origin, unitDir, distance, hitCall);
    return hitCall;
}
