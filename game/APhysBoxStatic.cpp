#include "APhysBoxStatic.h"

#include <PxRigidStatic.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/render/MeshUtilities.h>

APhysBoxStatic::APhysBoxStatic(PhysicsScene *physicsScene, const glm::vec3 &position, const glm::vec3 &halfSize)
        : m_halfSize(halfSize) {
    GetTransform().SetPosition(position);

    m_mesh = MeshBase(CreateBox(m_halfSize), GL_TRIANGLES);

    const physx::PxTransform transform{position.x, position.y, position.z};
    const physx::PxBoxGeometry geometry{m_halfSize.x, m_halfSize.y, m_halfSize.z};

    m_rigidbody = physicsScene->CreateStatic(transform, geometry);
    m_rigidbody->userData = this;
}

APhysBoxStatic::~APhysBoxStatic() {
    m_rigidbody->release();

    m_mesh = {};
}

void APhysBoxStatic::Update(const float deltaTime) {
    const physx::PxTransform transform = m_rigidbody->getGlobalPose();
    const glm::vec3 position{transform.p.x, transform.p.y, transform.p.z};
    const glm::quat rotation{transform.q.w, transform.q.x, transform.q.y, transform.q.z};

    m_modelMatrix = GetTransform().SetPosition(position).GetTranslationMatrix() * mat4_cast(rotation);
}

void APhysBoxStatic::Draw(Renderer &renderer) {
    renderer.DrawMesh(m_mesh, m_modelMatrix);
}
