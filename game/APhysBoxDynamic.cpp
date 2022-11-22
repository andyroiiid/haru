#include "APhysBoxDynamic.h"

#include <PxRigidDynamic.h>

#include <haru/physics/PhysicsSystem.h>
#include <haru/render/MeshUtilities.h>

std::unique_ptr<APhysBoxDynamic> APhysBoxDynamic::Create(
        PhysicsSystem *physics,
        const glm::vec3 &position,
        const glm::vec3 &halfSize,
        const glm::vec3 &velocity
) {
    auto entity = std::make_unique<APhysBoxDynamic>();
    entity->m_physics = physics;
    entity->GetTransform().SetPosition(position);
    entity->m_halfSize = halfSize;
    entity->m_velocity = velocity;
    return entity;
}

void APhysBoxDynamic::Awake() {
    m_mesh = MeshBase(CreateBox(m_halfSize), GL_TRIANGLES);

    const glm::vec3 &position = GetTransform().GetPosition();
    const physx::PxTransform transform{position.x, position.y, position.z};
    const physx::PxBoxGeometry geometry{m_halfSize.x, m_halfSize.y, m_halfSize.z};

    m_rigidbody = m_physics->CreateDynamic(transform, geometry);
    m_rigidbody->setLinearVelocity({m_velocity.x, m_velocity.y, m_velocity.z});
    m_rigidbody->userData = this;
}

void APhysBoxDynamic::Cleanup() {
    m_rigidbody->release();

    m_mesh = {};
}

void APhysBoxDynamic::Update(const float deltaTime) {
    const physx::PxTransform transform = m_rigidbody->getGlobalPose();
    const glm::vec3 position{transform.p.x, transform.p.y, transform.p.z};
    const glm::quat rotation{transform.q.w, transform.q.x, transform.q.y, transform.q.z};

    m_modelMatrix = GetTransform().SetPosition(position).GetTranslationMatrix() * mat4_cast(rotation);

    const physx::PxVec3 &velocity = m_rigidbody->getLinearVelocity();
    m_velocity.x = velocity.x;
    m_velocity.y = velocity.y;
    m_velocity.z = velocity.z;
}

void APhysBoxDynamic::Draw(Renderer &renderer) {
    renderer.DrawMesh(m_mesh, m_modelMatrix);
}
