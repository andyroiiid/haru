#include "APhysBoxStatic.h"

#include <PxRigidStatic.h>

#include <haru/physics/PhysicsSystem.h>
#include <haru/render/MeshUtilities.h>

std::unique_ptr<APhysBoxStatic> APhysBoxStatic::Create(
        PhysicsSystem *physics,
        const glm::vec3 &position,
        const glm::vec3 &halfSize
) {
    auto entity = std::make_unique<APhysBoxStatic>();
    entity->m_physics = physics;
    entity->GetTransform().SetPosition(position);
    entity->m_halfSize = halfSize;
    return entity;
}

void APhysBoxStatic::Awake() {
    m_mesh = MeshBase(CreateBox(m_halfSize), GL_TRIANGLES);

    const glm::vec3 &position = GetTransform().GetPosition();
    const physx::PxTransform transform{position.x, position.y, position.z};
    const physx::PxBoxGeometry geometry{m_halfSize.x, m_halfSize.y, m_halfSize.z};

    m_rigidbody = m_physics->CreateStatic(transform, geometry);
    m_rigidbody->userData = this;
}

void APhysBoxStatic::Cleanup() {
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
