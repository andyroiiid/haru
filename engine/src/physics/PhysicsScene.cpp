//
// Created by Andrew Huang on 11/22/2022.
//

#include "haru/physics/PhysicsScene.h"

#include <PxPhysicsAPI.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>

#include "haru/physics/PhysicsSystem.h"

PhysicsScene::PhysicsScene(PhysicsSystem *physicsSystem) : m_physics(physicsSystem->m_physics) {
    m_defaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    m_defaultMaterial = m_physics->createMaterial(0.8f, 0.8f, 0.2f);

    physx::PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = m_defaultCpuDispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    m_scene = m_physics->createScene(sceneDesc);

    if (physx::PxPvdSceneClient *pvdClient = m_scene->getScenePvdClient()) {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    m_controllerManager = PxCreateControllerManager(*m_scene);
}

PhysicsScene::~PhysicsScene() {
    PX_RELEASE(m_controllerManager)
    PX_RELEASE(m_scene)
    PX_RELEASE(m_defaultMaterial)
    PX_RELEASE(m_defaultCpuDispatcher)
}

bool PhysicsScene::Update(const float deltaTime, const float timeScale) {
    m_timeSinceLastTick += deltaTime * timeScale;

    const float scaledTimestep = m_fixedTimestep * timeScale;
    if (m_timeSinceLastTick < scaledTimestep) {
        return false;
    }

    m_scene->simulate(scaledTimestep);
    m_scene->fetchResults(true);
    m_timeSinceLastTick -= scaledTimestep;
    return true;
}

physx::PxController *PhysicsScene::CreateController(const glm::vec3 &position, float radius, float height) {
    physx::PxCapsuleControllerDesc desc;
    desc.position = {position.x, position.y, position.z};
    desc.invisibleWallHeight = height;
    desc.maxJumpHeight = height;
    desc.stepOffset = 0.3f;
    desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
    desc.material = m_defaultMaterial;
    desc.radius = radius;
    desc.height = height;
    desc.climbingMode = physx::PxCapsuleClimbingMode::eCONSTRAINED;
    return m_controllerManager->createController(desc);
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
        const physx::PxGeometry &geometry,
        float density
) {
    physx::PxRigidDynamic *actor = PxCreateDynamic(*m_physics, transform, geometry, *m_defaultMaterial, density);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRaycastBuffer PhysicsScene::Raycast(
        const physx::PxVec3 &origin,
        const physx::PxVec3 &unitDir,
        const float distance
) const {
    physx::PxRaycastBuffer buffer;
    m_scene->raycast(origin, unitDir, distance, buffer);
    return buffer;
}

physx::PxSweepBuffer PhysicsScene::Sweep(
        const physx::PxGeometry &geometry,
        const physx::PxTransform &pose,
        const physx::PxVec3 &unitDir,
        float distance
) const {
    physx::PxSweepBuffer buffer;
    m_scene->sweep(geometry, pose, unitDir, distance, buffer);
    return buffer;
}
