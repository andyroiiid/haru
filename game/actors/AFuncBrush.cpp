//
// Created by andyroiiid on 12/2/2022.
//

#include "actors/AFuncBrush.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/physics/PhysicsSystem.h>
#include <haru/render/Material.h>

#include "GameStatics.h"

AFuncBrush::AFuncBrush(const EntityBrushes &brushes) {
    PhysicsSystem *physicsSystem = GameStatics::GetPhysicsSystem();
    PhysicsScene *physicsScene = GameStatics::GetPhysicsScene();

    m_brushesRigidbody = physicsScene->CreateStatic(physx::PxTransform(0.0f, 0.0f, 0.0f));
    m_brushesRigidbody->userData = this;
    const physx::PxFilterData filterData = PhysicsFilterDataFromLayer(PHYSICS_LAYER_0);

    for (const auto &collider: brushes.Colliders) {
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

AFuncBrush::~AFuncBrush() {
    for (physx::PxShape *shape: m_brushShapes) {
        PX_RELEASE(shape)
    }

    for (physx::PxConvexMesh *collider: m_brushColliders) {
        PX_RELEASE(collider)
    }

    PX_RELEASE(m_brushesRigidbody)
}

void AFuncBrush::Draw(Renderer &renderer) {
    const glm::mat4 modelMatrix = GetTransform().GetMatrix();
    for (auto &facesAndTexture: m_facesAndTextures) {
        facesAndTexture.Draw(renderer, modelMatrix);
    }
}

void AFuncBrush::Use(APlayer *player, const physx::PxRaycastHit &hit) {
    Destroy();
}
