#include "actors/APointLight.h"

APointLight::APointLight(const glm::vec3 &position, const glm::vec3 &color, float range)
        : m_color(color),
          m_linear(PointLightAttenuationLinearFromRange(range)),
          m_quadratic(PointLightAttenuationQuadraticFromRange(range)) {
    GetTransform().SetPosition(position);
}

void APointLight::Draw(Renderer &renderer) {
    renderer.DrawPointLight(GetTransform().GetPosition(), m_color, m_linear, m_quadratic);
}
