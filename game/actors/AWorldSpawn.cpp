//
// Created by andyroiiid on 11/21/2022.
//

#include "actors/AWorldSpawn.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>

#include "GameStatics.h"

AWorldSpawn::AWorldSpawn(const EntityBrushes &brushes) {
    m_brushes = Brushes(brushes, PHYSICS_LAYER_0);
    const glm::vec3 &brushesOrigin = m_brushes.GetOrigin();

    GetTransform().SetPosition(brushesOrigin);

    m_brushesRigidbody = GameStatics::GetPhysicsScene()->CreateStatic(physx::PxTransform(brushesOrigin.x, brushesOrigin.y, brushesOrigin.z));
    m_brushes.AttachToRigidActor(m_brushesRigidbody);

    m_brushesRigidbody->userData = this;

    for (const auto &collider: brushes.Colliders) {
        for (const auto &vertex: collider) {
            m_geomMin.x = glm::min(vertex.x, m_geomMin.x);
            m_geomMin.y = glm::min(vertex.y, m_geomMin.y);
            m_geomMin.z = glm::min(vertex.z, m_geomMin.z);
            m_geomMax.x = glm::max(vertex.x, m_geomMax.x);
            m_geomMax.y = glm::max(vertex.y, m_geomMax.y);
            m_geomMax.z = glm::max(vertex.z, m_geomMax.z);
        }
    }
}

AWorldSpawn::~AWorldSpawn() {
    PX_RELEASE(m_brushesRigidbody)
}

void AWorldSpawn::Draw(Renderer &renderer) {
    renderer.SetWorldBounds(m_geomMin, m_geomMax);
    m_brushes.Draw(renderer, GetTransform().GetMatrix());
}
