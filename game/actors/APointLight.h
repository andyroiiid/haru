﻿#pragma once

#include "Actor.h"

class APointLight final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APointLight)

    explicit APointLight(
            const glm::vec3 &position,
            const glm::vec3 &color = glm::vec3{1.0f, 1.0f, 1.0f},
            float range = 20.0f
    );

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    glm::vec3 m_color{};
    float m_linear{};
    float m_quadratic{};
};