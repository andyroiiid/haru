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
            float flySpeed = 10.0f
    );

    ~APlayer() override;

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    glm::vec3 GetInputDirection();

    void Turn();

    void Move(float deltaTime);

    void CreateBox();

    void DestroyBox();

    PhysicsScene *m_physicsScene;
    Window *m_window;
    Scene *m_scene;

    physx::PxController *m_controller;

    float m_mouseSpeed;
    float m_flySpeed;

    bool m_prevLmb = false;
    bool m_prevRmb = false;
};
