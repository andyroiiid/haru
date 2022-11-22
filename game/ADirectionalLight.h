#pragma once

#include <memory>

#include "Actor.h"

class ADirectionalLight final : public Actor {
public:
    DEFINE_ACTOR_CLASS(ADirectionalLight)

    static std::unique_ptr<ADirectionalLight> Create(float intensity = 1.0f, float yaw = 30.0f, float pitch = 45.0f);

    void Awake() override;

    void Cleanup() override;

    void Update(float deltaTime) override;

    void Draw(Renderer &renderer) override;

private:
    float m_intensity{};
};

REGISTER_ACTOR_CLASS(ADirectionalLight)
