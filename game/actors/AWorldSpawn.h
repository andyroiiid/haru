//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include "Actor.h"
#include "map/Entity.h"
#include "map/Brushes.h"

namespace physx {
    class PxRigidStatic;
}

class AWorldSpawn : public Actor {
public:
    DEFINE_ACTOR_CLASS(AWorldSpawn)

    explicit AWorldSpawn(const EntityBrushes &brushes);

    ~AWorldSpawn() override;

    void Draw(Renderer &renderer) override;

private:
    Brushes m_brushes;
    physx::PxRigidStatic *m_brushesRigidbody;

    glm::vec3 m_geomMin{};
    glm::vec3 m_geomMax{};
};
