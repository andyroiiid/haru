#pragma once

#include <memory>

#include "Actor.h"

class PhysicsScene;

namespace physx {
    class PxRigidStatic;
}

class APhysBoxStatic final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APhysBoxStatic)

    APhysBoxStatic(PhysicsScene *physicsScene, const glm::vec3 &position, const glm::vec3 &halfSize);

    ~APhysBoxStatic() override;

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    glm::vec3 m_halfSize{};

    MeshBase m_mesh;
    physx::PxRigidStatic *m_rigidbody = nullptr;

    // not using m_transform because it doesn't support quaternion
    glm::mat4 m_modelMatrix{1.0f};
};
