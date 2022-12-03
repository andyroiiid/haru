//
// Created by andyroiiid on 12/2/2022.
//

#include "actors/AFuncBrush.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>

#include "GameStatics.h"

AFuncBrush::AFuncBrush(const EntityBrushes &brushes) {
    m_brushes = Brushes(brushes, PHYSICS_LAYER_0);

    m_brushesRigidbody = GameStatics::GetPhysicsScene()->CreateStatic(physx::PxTransform(0.0f, 0.0f, 0.0f));
    m_brushesRigidbody->userData = this;
    m_brushes.AttachToRigidActor(m_brushesRigidbody);
}

AFuncBrush::~AFuncBrush() {
    PX_RELEASE(m_brushesRigidbody)
}

void AFuncBrush::Draw(Renderer &renderer) {
    m_brushes.Draw(renderer, GetTransform().GetMatrix());
}

void AFuncBrush::Use(APlayer *player, const physx::PxRaycastHit &hit) {
    Destroy();
}
