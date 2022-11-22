#pragma once

#include <memory>

#include "Actor.h"

class PhysicsSystem;

namespace physx {
    class PxRigidStatic;
}

class APhysBoxStatic final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APhysBoxStatic)

    static std::unique_ptr<APhysBoxStatic> Create(
            PhysicsSystem *physics,
            const glm::vec3 &position,
            const glm::vec3 &halfSize
    );

    void Awake() override;

    void Cleanup() override;

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    PhysicsSystem *m_physics = nullptr;
    glm::vec3 m_halfSize{};

    MeshBase m_mesh;
    physx::PxRigidStatic *m_rigidbody = nullptr;

    // not using m_transform because it doesn't support quaternion
    glm::mat4 m_modelMatrix{1.0f};
};

REGISTER_ACTOR_CLASS(APhysBoxStatic)
