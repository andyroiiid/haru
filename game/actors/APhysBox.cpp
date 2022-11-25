#include "APhysBox.h"

#include <PxRigidDynamic.h>

#include <haru/physics/PhysicsScene.h>
#include <haru/render/MeshUtilities.h>

#include "../GameStatics.h"

APhysBox::APhysBox(
        const glm::vec3 &position,
        const glm::vec3 &halfSize,
        const glm::vec3 &velocity
) : m_halfSize(halfSize), m_velocity(velocity) {
    GetTransform().SetPosition(position);

    m_mesh = MeshBase(CreateBox(m_halfSize), GL_TRIANGLES);

    const physx::PxTransform transform{position.x, position.y, position.z};
    const physx::PxBoxGeometry geometry{m_halfSize.x, m_halfSize.y, m_halfSize.z};

    m_rigidbody = GameStatics::GetPhysicsScene()->CreateDynamic(transform, geometry);
    m_rigidbody->setLinearVelocity({m_velocity.x, m_velocity.y, m_velocity.z});
    m_rigidbody->userData = this;
}

APhysBox::~APhysBox() {
    m_rigidbody->release();

    m_mesh = {};
}

void APhysBox::Update(const float deltaTime) {
    const physx::PxTransform transform = m_rigidbody->getGlobalPose();
    const glm::vec3 position{transform.p.x, transform.p.y, transform.p.z};
    const glm::quat rotation{transform.q.w, transform.q.x, transform.q.y, transform.q.z};

    m_modelMatrix = GetTransform().SetPosition(position).GetTranslationMatrix() * mat4_cast(rotation);

    const physx::PxVec3 &velocity = m_rigidbody->getLinearVelocity();
    m_velocity.x = velocity.x;
    m_velocity.y = velocity.y;
    m_velocity.z = velocity.z;

    if (position.y < -1000.0f) {
        Destroy();
    }
}

void APhysBox::Draw(Renderer &renderer) {
    renderer.DrawMesh(m_mesh, m_modelMatrix);
}
