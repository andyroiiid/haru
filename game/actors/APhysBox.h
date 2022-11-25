#pragma once

#include "Actor.h"

namespace physx {
    class PxRigidDynamic;
}

class APhysBox final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APhysBox)

    APhysBox(
            const glm::vec3 &position,
            const glm::vec3 &halfSize,
            const glm::vec3 &velocity = {0.0f, 0.0f, 0.0f}
    );

    ~APhysBox() override;

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    glm::vec3 m_halfSize{};

    MeshBase m_mesh;
    physx::PxRigidDynamic *m_rigidbody = nullptr;

    // not using m_transform because it doesn't support quaternion
    glm::mat4 m_modelMatrix{1.0f};
    glm::vec3 m_velocity{};
};
