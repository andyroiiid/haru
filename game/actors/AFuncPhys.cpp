//
// Created by andyroiiid on 12/2/2022.
//

#include "AFuncPhys.h"

#include <PxRigidDynamic.h>
#include <foundation/PxAllocator.h>
#include <extensions/PxRigidBodyExt.h>

#include <haru/physics/PhysicsScene.h>

#include "GameStatics.h"

AFuncPhys::AFuncPhys(const EntityBrushes &brushes) {
    m_brushes = Brushes(brushes, PHYSICS_LAYER_0);
    const glm::vec3 &brushesOrigin = m_brushes.GetOrigin();

    GetTransform().SetPosition(brushesOrigin);

    m_brushesRigidbody = GameStatics::GetPhysicsScene()->CreateDynamic(physx::PxTransform(brushesOrigin.x, brushesOrigin.y, brushesOrigin.z));
    m_brushes.AttachToRigidActor(m_brushesRigidbody);
    physx::PxRigidBodyExt::updateMassAndInertia(*m_brushesRigidbody, 10.0f);

    m_brushesRigidbody->userData = this;
}

AFuncPhys::~AFuncPhys() {
    PX_RELEASE(m_brushesRigidbody)
}

void AFuncPhys::FixedUpdate(float fixedDeltaTime) {
    const physx::PxTransform transform = m_brushesRigidbody->getGlobalPose();
    const glm::vec3 position{transform.p.x, transform.p.y, transform.p.z};
    const glm::quat rotation{transform.q.w, transform.q.x, transform.q.y, transform.q.z};

    m_modelMatrix = GetTransform().SetPosition(position).GetTranslationMatrix() * mat4_cast(rotation);
}

void AFuncPhys::Draw(Renderer &renderer) {
    m_brushes.Draw(renderer, m_modelMatrix);
}
