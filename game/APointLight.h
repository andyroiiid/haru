#pragma once

#include <memory>

#include "Actor.h"

class APointLight final : public Actor {
public:
    DEFINE_ACTOR_CLASS(APointLight)

    enum class Range {
        R7,
        R13,
        R20,
        R32,
        R50,
        R65,
        R100,
        R160,
        R200,
        R325,
        R600,
        R3250,
    };

    explicit APointLight(
            const glm::vec3 &position,
            Range range = Range::R20,
            const glm::vec3 &color = glm::vec3{1.0f, 1.0f, 1.0f}
    );

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    Range m_range{};
    glm::vec3 m_color{};
};
