//
// Created by andyroiiid on 11/21/2022.
//

#include "ALevelGeometry.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/physics/PhysicsSystem.h>
#include <haru/render/Material.h>

#include "../GameStatics.h"

ALevelGeometry::ALevelGeometry(const EntityBrushes &brushes) {
    const physx::PxTransform rigidbodyTransform(physx::PxVec3{0.0f, 0.0f, 0.0f});
    PhysicsSystem *physicsSystem = GameStatics::GetPhysicsSystem();
    PhysicsScene *physicsScene = GameStatics::GetPhysicsScene();

    for (const auto &collider: brushes.Colliders) {
        for (const auto &vertex: collider) {
            m_geomMin.x = glm::min(vertex.x, m_geomMin.x);
            m_geomMin.y = glm::min(vertex.y, m_geomMin.y);
            m_geomMin.z = glm::min(vertex.z, m_geomMin.z);
            m_geomMax.x = glm::max(vertex.x, m_geomMax.x);
            m_geomMax.y = glm::max(vertex.y, m_geomMax.y);
            m_geomMax.z = glm::max(vertex.z, m_geomMax.z);
        }

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

ALevelGeometry::~ALevelGeometry() {
    for (physx::PxRigidStatic *rigidbody: m_brushRigidbodies) {
        PX_RELEASE(rigidbody)
    }

    for (physx::PxConvexMesh *collider: m_brushColliders) {
        PX_RELEASE(collider)
    }
}

void ALevelGeometry::Draw(Renderer &renderer) {
    renderer.SetWorldBounds(m_geomMin, m_geomMax);
    const glm::mat4 modelMatrix = GetTransform().GetMatrix();
    for (auto &meshAndMaterial: m_meshesAndMaterials) {
        meshAndMaterial.Draw(renderer, modelMatrix);
    }
}

ALevelGeometry::MeshAndTexture::MeshAndTexture(const std::vector<VertexBase> &vertices, const std::string &textureName)
        : m_mesh(vertices, GL_TRIANGLES),
          m_texture(Texture::FromFile("textures/" + textureName + ".png")) {
}

void ALevelGeometry::MeshAndTexture::Draw(Renderer &renderer, const glm::mat4 &modelMatrix) {
    Material material{&m_texture};
    renderer.DrawMesh(m_mesh, modelMatrix, &material);
}
