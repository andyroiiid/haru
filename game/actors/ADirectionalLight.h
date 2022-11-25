#pragma once

#include <glm/trigonometric.hpp>

#include "Actor.h"

class ADirectionalLight final : public Actor {
public:
    DEFINE_ACTOR_CLASS(ADirectionalLight)

    explicit ADirectionalLight(float intensity = 1.0f, float yaw = glm::radians(30.0f), float pitch = glm::radians(45.0f));

    void Draw(Renderer &renderer) override;

private:
    float m_intensity{};
};
