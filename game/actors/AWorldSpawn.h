//
// Created by andyroiiid on 11/21/2022.
//

#pragma once

#include "Actor.h"
#include "map/Entity.h"
#include "map/FacesAndTexture.h"

namespace physx {
    class PxConvexMesh;

    class PxShape;

    class PxRigidStatic;
}

class AWorldSpawn : public Actor {
public:
    DEFINE_ACTOR_CLASS(AWorldSpawn)

    explicit AWorldSpawn(const EntityBrushes &brushes);

    ~AWorldSpawn() override;

    void Draw(Renderer &renderer) override;

private:
    std::vector<FacesAndTexture> m_facesAndTextures;
    std::vector<physx::PxConvexMesh *> m_brushColliders;
    std::vector<physx::PxShape *> m_brushShapes;
    physx::PxRigidStatic *m_brushesRigidbody;

    glm::vec3 m_geomMin{};
    glm::vec3 m_geomMax{};
};
