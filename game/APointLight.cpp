#include "APointLight.h"

APointLight::APointLight(const glm::vec3 &position, APointLight::Range range, const glm::vec3 &color)
        : m_range(range), m_color(color) {
    GetTransform().SetPosition(position);
}

void APointLight::Update(float deltaTime) {
}

void APointLight::Draw(Renderer &renderer) {
    // point light attenuation from https://wiki.ogre3d.org/-Point+Light+Attenuation
    static constexpr glm::vec2 LIGHT_PARAMS[] = {
            {0.7f,    1.8f}, // R7
            {0.35f,   0.44f}, // R13
            {0.22f,   0.20f}, // R20
            {0.14f,   0.07f}, // R32
            {0.09f,   0.032f}, // R50
            {0.07f,   0.017f}, // R65
            {0.045f,  0.0075f}, // R100
            {0.027f,  0.0028f}, // R160
            {0.022f,  0.0019f}, // R200
            {0.014f,  0.0007f}, // R325
            {0.007f,  0.0002f}, // R600
            {0.0014f, 0.000007f}, // R3250
    };
    const glm::vec2 &param = LIGHT_PARAMS[static_cast<int>(m_range)];
    renderer.DrawPointLight(GetTransform().GetPosition(), m_color, param.x, param.y);
}
