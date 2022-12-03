//
// Created by andyroiiid on 12/2/2022.
//

#include "actors/AFuncBrush.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/physics/PhysicsSystem.h>
#include <haru/render/Material.h>

#include "GameStatics.h"

AFuncBrush::AFuncBrush(const EntityBrushes &brushes) {
    const physx::PxTransform rigidbodyTransform(physx::PxVec3{0.0f, 0.0f, 0.0f});
    PhysicsSystem *physicsSystem = GameStatics::GetPhysicsSystem();
    PhysicsScene *physicsScene = GameStatics::GetPhysicsScene();

    for (const auto &collider: brushes.Colliders) {
        physx::PxConvexMesh *brushCollider = physicsSystem->CreateConvexMesh(collider.size(), collider.data());
        m_brushColliders.push_back(brushCollider);

        physx::PxRigidStatic *brushRigidbody = physicsScene->CreateStatic(rigidbodyTransform, physx::PxConvexMeshGeometry(brushCollider));
        brushRigidbody->userData = this;
        m_brushRigidbodies.push_back(brushRigidbody);
    }

    for (const auto &[textureName, vertices]: brushes.TextureToVertices) {
        m_meshesAndMaterials.emplace_back(vertices, textureName);
    }
}

AFuncBrush::~AFuncBrush() {
    for (physx::PxRigidStatic *rigidbody: m_brushRigidbodies) {
        PX_RELEASE(rigidbody)
    }

    for (physx::PxConvexMesh *collider: m_brushColliders) {
        PX_RELEASE(collider)
    }
}

void AFuncBrush::Draw(Renderer &renderer) {
    const glm::mat4 modelMatrix = GetTransform().GetMatrix();
    for (auto &meshAndMaterial: m_meshesAndMaterials) {
        meshAndMaterial.Draw(renderer, modelMatrix);
    }
}

void AFuncBrush::Use(APlayer *player, const physx::PxRaycastHit &hit) {
    Destroy();
}

AFuncBrush::MeshAndTexture::MeshAndTexture(const std::vector<VertexBase> &vertices, const std::string &textureName)
        : m_mesh(vertices, GL_TRIANGLES),
          m_texture(Texture::FromFile("textures/" + textureName + ".png")) {
}

void AFuncBrush::MeshAndTexture::Draw(Renderer &renderer, const glm::mat4 &modelMatrix) {
    Material material{&m_texture};
    renderer.DrawMesh(m_mesh, modelMatrix, &material);
}
