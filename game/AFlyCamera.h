#pragma once

#include <memory>

#include "Actor.h"

class PhysicsSystem;

class Window;

class Scene;

struct CameraConfig {
    float MouseSpeed = 0.001f;
    float FlySpeed = 10.0f;

    float FieldOfViewDegrees = 90.0f;
    float NearPlane = 0.01f;
    float FarPlane = 1000.0f;
};

class AFlyCamera final : public Actor {
public:
    DEFINE_ACTOR_CLASS(AFlyCamera)

    AFlyCamera(
            PhysicsSystem *physics,
            Window *window,
            Scene *scene,
            const glm::vec3 &position = {0, 0, 5},
            float yaw = 0.0f,
            const CameraConfig &cameraConfig = {}
    );

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    PhysicsSystem *m_physics = nullptr;
    Window *m_window = nullptr;
    Scene *m_scene = nullptr;

    CameraConfig m_cameraConfig;

    bool m_prevF = false;
    bool m_flashLight = false;

    bool m_prevLmb = false;
    bool m_prevRmb = false;
};
