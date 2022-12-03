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
    const glm::vec3 &brushesOrigin = m_brushes.GetOrigin();

    GetTransform().SetPosition(brushesOrigin);

    m_brushesRigidbody = GameStatics::GetPhysicsScene()->CreateStatic(physx::PxTransform(brushesOrigin.x, brushesOrigin.y, brushesOrigin.z));
    m_brushes.AttachToRigidActor(m_brushesRigidbody);

    m_brushesRigidbody->userData = this;
}

AFuncBrush::~AFuncBrush() {
    PX_RELEASE(m_brushesRigidbody)
}

void AFuncBrush::Draw(Renderer &renderer) {
    m_brushes.Draw(renderer, GetTransform().GetMatrix());
}
