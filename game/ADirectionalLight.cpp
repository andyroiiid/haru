#include "ADirectionalLight.h"

std::unique_ptr<ADirectionalLight>
ADirectionalLight::Create(const float intensity, const float yaw, const float pitch) {
    auto entity = std::make_unique<ADirectionalLight>();
    entity->m_intensity = intensity;
    entity->GetTransform()
            .RotateY(yaw)
            .RotateX(pitch)
            .ClampPitch();
    return entity;
}

void ADirectionalLight::Awake() {
}

void ADirectionalLight::Cleanup() {
}

void ADirectionalLight::Update(float deltaTime) {
}

void ADirectionalLight::Draw(Renderer &renderer) {
    renderer.SetDirectionalLight(GetTransform().GetForwardVector(), m_intensity);
}
