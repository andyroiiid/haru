//
// Created by andyroiiid on 11/26/2022.
//

#include "actors/APropStatic.h"

#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>
#include <geometry/PxTriangleMesh.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/physics/PhysicsSystem.h>
#include <haru/render/Material.h>

#include "mdl/MdlLoader.h"
#include "GameStatics.h"

APropStatic::APropStatic(const std::string &model, const glm::vec3 &origin) {
    PhysicsSystem *physicsSystem = GameStatics::GetPhysicsSystem();
    PhysicsScene *physicsScene = GameStatics::GetPhysicsScene();

    MdlLoader loader(model);
    m_texture = loader.GetTexture();
    m_mesh = loader.GetMesh();
    std::vector<physx::PxVec3> colliderTriangles = loader.GetColliderTriangles();

    GetTransform().SetPosition(origin);

    m_collider = physicsSystem->CreateTriangleMesh(colliderTriangles.size(), colliderTriangles.data());
    m_rigidbody = physicsScene->CreateStatic(physx::PxTransform(origin.x, origin.y, origin.z), physx::PxTriangleMeshGeometry(m_collider));
}

APropStatic::~APropStatic() {
    PX_RELEASE(m_rigidbody)
    PX_RELEASE(m_collider)
}

void APropStatic::Draw(Renderer &renderer) {
    Material material{&m_texture};
    renderer.DrawMesh(m_mesh, GetTransform().GetMatrix(), &material);
}
