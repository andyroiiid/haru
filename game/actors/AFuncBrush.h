//
// Created by andyroiiid on 12/2/2022.
//

#pragma once

#include "Actor.h"
#include "map/Entity.h"
#include "map/Brushes.h"

namespace physx {
    class PxRigidStatic;
}

class AFuncBrush : public Actor {
public:
    DEFINE_ACTOR_CLASS(AFuncBrush)

    explicit AFuncBrush(const EntityBrushes &brushes);

    ~AFuncBrush() override;

    void Draw(Renderer &renderer) override;

    void Use(APlayer *player, const physx::PxRaycastHit &hit) override;

private:
    Brushes m_brushes;
    physx::PxRigidStatic *m_brushesRigidbody;
};
