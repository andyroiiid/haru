//
// Created by Andrew Huang on 11/22/2022.
//

#pragma once

#include <glm/vec3.hpp>
#include <characterkinematic/PxExtended.h>
#include <geometry/PxGeometry.h>
#include <PxQueryReport.h>

#include "haru/core/MoveOnly.h"
#include "haru/physics/PhysicsLayer.h"

class PhysicsSystem;

namespace physx {
    class PxPhysics;

    class PxDefaultCpuDispatcher;

    class PxMaterial;

    class PxScene;

    class PxController;

    class PxControllerManager;

    class PxRigidStatic;

    class PxRigidDynamic;
}

class PhysicsScene {
public:
    NO_MOVE_OR_COPY(PhysicsScene)

    explicit PhysicsScene(PhysicsSystem *physicsSystem);

    ~PhysicsScene();

    bool Update(float deltaTime, float timeScale);

    [[nodiscard]] float GetFixedTimestep() const { return m_fixedTimestep; }

    [[nodiscard]] float GetFixedUpdateTimeError() const { return m_timeSinceLastTick; }

    physx::PxController *CreateController(
            const glm::vec3 &position,
            float radius,
            float height,
            PhysicsLayer queryLayer = PHYSICS_LAYER_1
    );

    physx::PxShape *CreateShape(
            const physx::PxGeometry &geometry,
            bool isExclusive
    );

    physx::PxRigidStatic *CreateStatic(
            const physx::PxTransform &transform
    );

    physx::PxRigidStatic *CreateStatic(
            const physx::PxTransform &transform,
            const physx::PxGeometry &geometry,
            PhysicsLayer queryLayer = PHYSICS_LAYER_0
    );

    physx::PxRigidDynamic *CreateDynamic(
            const physx::PxTransform &transform
    );

    physx::PxRigidDynamic *CreateDynamic(
            const physx::PxTransform &transform,
            const physx::PxGeometry &geometry,
            PhysicsLayer queryLayer = PHYSICS_LAYER_0,
            float density = 1.0f
    );

    [[nodiscard]] physx::PxRaycastBuffer Raycast(
            const physx::PxVec3 &origin,
            const physx::PxVec3 &unitDir,
            float distance,
            PhysicsLayer layer
    ) const;

    [[nodiscard]] physx::PxSweepBuffer Sweep(
            const physx::PxGeometry &geometry,
            const physx::PxTransform &pose,
            const physx::PxVec3 &unitDir,
            float distance,
            PhysicsLayer layer
    ) const;

private:
    physx::PxPhysics *m_physics = nullptr;

    physx::PxDefaultCpuDispatcher *m_defaultCpuDispatcher = nullptr;
    physx::PxMaterial *m_defaultMaterial = nullptr;
    physx::PxScene *m_scene = nullptr;
    physx::PxControllerManager *m_controllerManager = nullptr;

    float m_fixedTimestep = 0.02f;
    float m_timeSinceLastTick = 0.0f;
};
