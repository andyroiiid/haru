//
// Created by andyroiiid on 11/26/2022.
//

#pragma once

#include "Actor.h"

#include <haru/render/Texture.h>
#include <haru/render/MeshBase.h>

namespace physx {
    class PxTriangleMesh;

    class PxRigidStatic;
}

class APropStatic : public Actor {
public:
    DEFINE_ACTOR_CLASS(APropStatic)

    APropStatic(const std::string &model, const glm::vec3 &origin);

    ~APropStatic() override;

    void Draw(Renderer &renderer) override;

private:
    Texture m_texture;
    MeshBase m_mesh;

    physx::PxTriangleMesh *m_collider = nullptr;
    physx::PxRigidStatic *m_rigidbody = nullptr;
};
