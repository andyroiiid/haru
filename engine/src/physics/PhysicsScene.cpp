//
// Created by Andrew Huang on 11/22/2022.
//

#include "haru/physics/PhysicsScene.h"

#include <PxPhysicsAPI.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <tracy/Tracy.hpp>

#include "haru/physics/PhysicsSystem.h"

PhysicsScene::PhysicsScene(PhysicsSystem *physicsSystem) : m_physics(physicsSystem->m_physics) {
    m_defaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    m_defaultMaterial = m_physics->createMaterial(0.8f, 0.4f, 0.2f);

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
    ZoneScoped;

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

physx::PxController *PhysicsScene::CreateController(const glm::vec3 &position, float radius, float height, PhysicsLayer queryLayer) {
    physx::PxCapsuleControllerDesc desc;
    desc.position = {position.x, position.y, position.z};
    desc.material = m_defaultMaterial;
    // https://nvidia-omniverse.github.io/PhysX/physx/5.1.0/docs/CharacterControllers.html#character-volume
    desc.radius = radius;
    desc.height = height;

    physx::PxController *controller = m_controllerManager->createController(desc);

    physx::PxRigidDynamic *rigidbody = controller->getActor();
    PhysicsSetQueryLayer(rigidbody, queryLayer);

    return controller;
}

physx::PxShape *PhysicsScene::CreateShape(const physx::PxGeometry &geometry, bool isExclusive) {
    return m_physics->createShape(geometry, *m_defaultMaterial, isExclusive);
}

physx::PxRigidStatic *PhysicsScene::CreateStatic(const physx::PxTransform &transform) {
    physx::PxRigidStatic *actor = m_physics->createRigidStatic(transform);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRigidStatic *PhysicsScene::CreateStatic(
        const physx::PxTransform &transform,
        const physx::PxGeometry &geometry,
        PhysicsLayer queryLayer
) {
    physx::PxRigidStatic *actor = PxCreateStatic(*m_physics, transform, geometry, *m_defaultMaterial);
    PhysicsSetQueryLayer(actor, queryLayer);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRigidDynamic *PhysicsScene::CreateDynamic(const physx::PxTransform &transform) {
    physx::PxRigidDynamic *actor = m_physics->createRigidDynamic(transform);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRigidDynamic *PhysicsScene::CreateDynamic(
        const physx::PxTransform &transform,
        const physx::PxGeometry &geometry,
        PhysicsLayer queryLayer,
        float density
) {
    physx::PxRigidDynamic *actor = PxCreateDynamic(*m_physics, transform, geometry, *m_defaultMaterial, density);
    PhysicsSetQueryLayer(actor, queryLayer);
    m_scene->addActor(*actor);
    return actor;
}

physx::PxRaycastBuffer PhysicsScene::Raycast(
        const physx::PxVec3 &origin,
        const physx::PxVec3 &unitDir,
        const float distance,
        PhysicsLayer layer
) const {
    physx::PxQueryFilterData queryFilterData;
    queryFilterData.data = PhysicsFilterDataFromLayer(layer);

    physx::PxRaycastBuffer buffer;
    m_scene->raycast(origin, unitDir, distance, buffer, physx::PxHitFlag::eDEFAULT, queryFilterData);
    return buffer;
}

physx::PxSweepBuffer PhysicsScene::Sweep(
        const physx::PxGeometry &geometry,
        const physx::PxTransform &pose,
        const physx::PxVec3 &unitDir,
        float distance,
        PhysicsLayer layer
) const {
    physx::PxQueryFilterData queryFilterData;
    queryFilterData.data = PhysicsFilterDataFromLayer(layer);

    physx::PxSweepBuffer buffer;
    m_scene->sweep(geometry, pose, unitDir, distance, buffer, physx::PxHitFlag::eDEFAULT, queryFilterData);
    return buffer;
}
