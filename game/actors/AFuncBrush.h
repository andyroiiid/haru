//
// Created by andyroiiid on 12/2/2022.
//

#pragma once

#include "Actor.h"
#include "map/Entity.h"
#include "map/FacesAndTexture.h"

namespace physx {
    class PxConvexMesh;

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
    std::vector<FacesAndTexture> m_facesAndTextures;
    std::vector<physx::PxConvexMesh *> m_brushColliders;
    std::vector<physx::PxRigidStatic *> m_brushRigidbodies;
};
