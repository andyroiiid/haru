#include "ADirectionalLight.h"

ADirectionalLight::ADirectionalLight(float intensity, float yaw, float pitch)
        : m_intensity(intensity) {
    GetTransform()
            .RotateY(yaw)
            .RotateX(pitch)
            .ClampPitch();
}

void ADirectionalLight::Draw(Renderer &renderer) {
    renderer.SetDirectionalLight(GetTransform().GetForwardVector(), m_intensity);
}
