#pragma once

#include "Actor.h"

namespace physx {
    class PxController;
}

class APlayer final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APlayer)

    explicit APlayer(
            const glm::vec3 &position = {0, 0, 5},
            float yaw = 0.0f,
            float mouseSpeed = 0.001f
    );

    ~APlayer() override;

    void Update(float deltaTime) override;

    void FixedUpdate(float fixedDeltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    void ReadMovementInput();

    void CalcHorizontalAcceleration(const glm::vec3 &direction, float acceleration, float drag);

    void UpdateAcceleration();

    physx::PxController *m_controller;

    float m_mouseSpeed;

    float m_groundAcceleration = 50.0f;
    float m_gravity = 20.0f;
    float m_groundDrag = 10.0f;

    glm::vec3 m_movementInput{};

    glm::vec3 m_velocity{};
    glm::vec3 m_acceleration{};

    glm::vec3 m_previousPosition{};

    bool m_prevLmb = false;

    glm::vec3 m_lastHitPosition{};
    glm::vec3 m_lastHitNormal{};
};
