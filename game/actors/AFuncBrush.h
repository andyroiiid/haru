//
// Created by andyroiiid on 12/2/2022.
//

#pragma once

#include <glm/vec3.hpp>
#include <haru/core/MoveOnly.h>
#include <haru/render/Texture.h>

namespace physx {
    class PxConvexMesh;

    class PxRigidStatic;
}

#include "Actor.h"
#include "map/Entity.h"

class AFuncBrush : public Actor {
public:
    DEFINE_ACTOR_CLASS(AFuncBrush)

    explicit AFuncBrush(const EntityBrushes &brushes);

    ~AFuncBrush() override;

    void Draw(Renderer &renderer) override;

    void Use(APlayer *player, const physx::PxRaycastHit &hit) override;

private:
    class MeshAndTexture {
    public:
        MOVE_ONLY(MeshAndTexture)

        MeshAndTexture(const std::vector<VertexBase> &vertices, const std::string &textureName);

        void Draw(Renderer &renderer, const glm::mat4 &modelMatrix);

    private:
        MeshBase m_mesh;
        Texture m_texture;
    };

    std::vector<MeshAndTexture> m_meshesAndMaterials;

    std::vector<physx::PxConvexMesh *> m_brushColliders;
    std::vector<physx::PxRigidStatic *> m_brushRigidbodies;
};
