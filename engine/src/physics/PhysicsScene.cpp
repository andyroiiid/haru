//
// Created by Andrew Huang on 11/22/2022.
//

#include "haru/physics/PhysicsScene.h"

#include <PxPhysicsAPI.h>

PhysicsScene::PhysicsScene(PhysicsSystem *physicsSystem)
        : m_physicsSystem(physicsSystem),
          m_physics(physicsSystem->m_physics),
          m_defaultMaterial(physicsSystem->m_defaultMaterial) {
    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = m_physicsSystem->m_dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_scene = m_physics->createScene(sceneDesc);

    if (physx::PxPvdSceneClient *pvdClient = m_scene->getScenePvdClient()) {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

PhysicsScene::~PhysicsScene() {
    PX_RELEASE(m_scene)
}

void PhysicsScene::Update(const float deltaTime, const float timeScale) {
    m_timeSinceLastTick += deltaTime * timeScale;

    static constexpr float FIXED_TIMESTEP = 0.02f;
    const float scaledTimestep = FIXED_TIMESTEP * timeScale;

    if (m_timeSinceLastTick >= scaledTimestep) {
        m_scene->simulate(scaledTimestep);
        m_scene->fetchResults(true);
        m_timeSinceLastTick -= scaledTimestep;
    }
}

physx::PxRigidStatic *PhysicsScene::CreateStatic(
        const physx::PxTransform &transform,
        const physx::PxGeometry &geometry
) {
    physx::PxRigidStatic *actor = PxCreateStatic(*m_physics, transform, geometry, *m_defaultMaterial);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRigidDynamic *PhysicsScene::CreateDynamic(
        const physx::PxTransform &transform,
        const physx::PxGeometry &geometry
) {
    physx::PxRigidDynamic *actor = PxCreateDynamic(*m_physics, transform, geometry, *m_defaultMaterial, 1.0f);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRaycastBuffer PhysicsScene::Raycast(
        const physx::PxVec3 &origin,
        const physx::PxVec3 &unitDir,
        const float distance
) const {
    physx::PxRaycastBuffer hitCall;
    m_scene->raycast(origin, unitDir, distance, hitCall);
    return hitCall;
}
