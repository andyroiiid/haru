//
// Created by andyroiiid on 12/2/2022.
//

#include "Brushes.h"

#include <PxRigidActor.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/physics/PhysicsSystem.h>

#include "GameStatics.h"

Brushes::Brushes(const EntityBrushes &brushes, PhysicsLayer layer) {
    for (const auto &[textureName, vertices]: brushes.TextureToVertices) {
        m_facesAndTextures.emplace_back(vertices, textureName);
    }

    PhysicsSystem *physicsSystem = GameStatics::GetPhysicsSystem();
    PhysicsScene *physicsScene = GameStatics::GetPhysicsScene();

    const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(layer);

    for (const auto &collider: brushes.Colliders) {
        physx::PxConvexMesh *brushCollider = physicsSystem->CreateConvexMesh(collider.size(), collider.data());
        m_brushColliders.push_back(brushCollider);

        physx::PxShape *brushShape = physicsScene->CreateShape(physx::PxConvexMeshGeometry(brushCollider), true);
        brushShape->setQueryFilterData(filterData);
        m_brushShapes.push_back(brushShape);
    }
}

Brushes::~Brushes() {
    for (physx::PxShape *shape: m_brushShapes) {
        PX_RELEASE(shape)
    }

    for (physx::PxConvexMesh *collider: m_brushColliders) {
        PX_RELEASE(collider)
    }
}

void Brushes::AttachToRigidActor(physx::PxRigidActor *actor) {
    for (const auto &shape: m_brushShapes) {
        actor->attachShape(*shape);
    }
}

void Brushes::Draw(Renderer &renderer, const glm::mat4 &modelMatrix) {
    for (auto &facesAndTexture: m_facesAndTextures) {
        facesAndTexture.Draw(renderer, modelMatrix);
    }
}
