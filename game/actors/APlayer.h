#pragma once

#include "Actor.h"

#include <haru/math/Random.h>

class PhysicsScene;

class Window;

class Scene;

namespace physx {
    class PxController;
}

class APointLight;

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

    PhysicsScene *m_physicsScene;
    Window *m_window;

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
    static constexpr int MAX_NUM_POINT_LIGHTS = 4;
    APointLight *m_pointLights[MAX_NUM_POINT_LIGHTS]{};
    int m_nextPointLight = 0;
    Random m_random;
};
