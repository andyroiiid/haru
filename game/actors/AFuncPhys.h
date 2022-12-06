//
// Created by andyroiiid on 12/2/2022.
//

#pragma once

#include "Actor.h"
#include "map/Entity.h"
#include "map/Brushes.h"

namespace physx {
    class PxRigidDynamic;
}

class AFuncPhys : public Actor {
public:
    DEFINE_ACTOR_CLASS(AFuncPhys)

    explicit AFuncPhys(const EntityBrushes &brushes);

    ~AFuncPhys() override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw(Renderer &renderer) override;

    void Use(APlayer *player, const physx::PxRaycastHit &hit) override;

private:
    Brushes m_brushes;
    physx::PxRigidDynamic *m_brushesRigidbody;

    glm::mat4 m_modelMatrix{1.0f};
};
