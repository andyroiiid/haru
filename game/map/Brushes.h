//
// Created by andyroiiid on 12/2/2022.
//

#pragma once

#include <haru/physics/PhysicsLayer.h>

#include "map/Entity.h"
#include "map/FacesAndTexture.h"

namespace physx {
    class PxConvexMesh;

    class PxShape;

    class PxRigidActor;
}

class Brushes {
public:
    MOVE_ONLY(Brushes)

    Brushes() = default;

    explicit Brushes(const EntityBrushes &brushes, PhysicsLayer layer = PHYSICS_LAYER_0);

    ~Brushes();

    void AttachToRigidActor(physx::PxRigidActor *actor);

    void Draw(Renderer &renderer, const glm::mat4 &modelMatrix);

private:
    std::vector<FacesAndTexture> m_facesAndTextures;
    std::vector<physx::PxConvexMesh *> m_brushColliders;
    std::vector<physx::PxShape *> m_brushShapes;
};
