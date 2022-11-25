#pragma once

#include "Actor.h"

class PhysicsScene;

class Window;

class Scene;

namespace physx {
    class PxController;
}

class APlayer final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APlayer)

    explicit APlayer(
            const glm::vec3 &position = {0, 0, 5},
            float yaw = 0.0f,
            float mouseSpeed = 0.001f,
            float flySpeed = 5.0f
    );

    ~APlayer() override;

    void Update(float deltaTime) override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    glm::vec3 GetInputDirection();

    PhysicsScene *m_physicsScene;
    Window *m_window;

    physx::PxController *m_controller;

    float m_mouseSpeed;
    float m_flySpeed;

    glm::vec3 m_inputVelocity{};

    glm::vec3 m_previousPosition{};
    glm::vec3 m_projectedVelocity{};
};
