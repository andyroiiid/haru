//
// Created by andyroiiid on 11/21/2022.
//

#include "actors/AWorldSpawn.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/physics/PhysicsSystem.h>
#include <haru/render/Material.h>

#include "GameStatics.h"

AWorldSpawn::AWorldSpawn(const EntityBrushes &brushes) {
    PhysicsSystem *physicsSystem = GameStatics::GetPhysicsSystem();
    PhysicsScene *physicsScene = GameStatics::GetPhysicsScene();

    m_brushesRigidbody = physicsScene->CreateStatic(physx::PxTransform(0.0f, 0.0f, 0.0f));
    m_brushesRigidbody->userData = this;
    const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(PHYSICS_LAYER_0);

    for (const auto &collider: brushes.Colliders) {
        for (const auto &vertex: collider) {
            m_geomMin.x = glm::min(vertex.x, m_geomMin.x);
            m_geomMin.y = glm::min(vertex.y, m_geomMin.y);
            m_geomMin.z = glm::min(vertex.z, m_geomMin.z);
            m_geomMax.x = glm::max(vertex.x, m_geomMax.x);
            m_geomMax.y = glm::max(vertex.y, m_geomMax.y);
            m_geomMax.z = glm::max(vertex.z, m_geomMax.z);
        }

        physx::PxConvexMesh *brushCollider = physicsSystem->CreateConvexMesh(collider.size(), collider.data());
        m_brushColliders.push_back(brushCollider);

        physx::PxShape *brushShape = physicsScene->CreateShape(physx::PxConvexMeshGeometry(brushCollider), true);
        brushShape->setQueryFilterData(filterData);
        m_brushShapes.push_back(brushShape);

        m_brushesRigidbody->attachShape(*brushShape);
    }

    for (const auto &[textureName, vertices]: brushes.TextureToVertices) {
        m_facesAndTextures.emplace_back(vertices, textureName);
    }
}

AWorldSpawn::~AWorldSpawn() {
    for (physx::PxShape *shape: m_brushShapes) {
        PX_RELEASE(shape)
    }

    for (physx::PxConvexMesh *collider: m_brushColliders) {
        PX_RELEASE(collider)
    }

    PX_RELEASE(m_brushesRigidbody)
}

void AWorldSpawn::Draw(Renderer &renderer) {
    renderer.SetWorldBounds(m_geomMin, m_geomMax);
    const glm::mat4 modelMatrix = GetTransform().GetMatrix();
    for (auto &facesAndTexture: m_facesAndTextures) {
        facesAndTexture.Draw(renderer, modelMatrix);
    }
}
