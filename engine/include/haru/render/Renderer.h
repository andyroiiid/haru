//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "haru/render/MeshPositionOnly.h"
#include "haru/render/MeshBase.h"

struct Material;

// point light attenuation from https://wiki.ogre3d.org/-Point+Light+Attenuation
// poly-fitted expression: log(y) = A * log(x) + B
static inline float PointLightAttenuationLinearFromRange(float range) {
    return glm::exp(-1.00971225f * glm::log(range) + 1.54554086f);
}

static inline float PointLightAttenuationQuadraticFromRange(float range) {
    return glm::exp(-2.01920616f * glm::log(range) + 4.41212873f);
}

class Renderer {
public:
    MOVE_ONLY(Renderer)

    Renderer() = default;

    virtual ~Renderer() = default;

    void Resize(const glm::ivec2 &size) {
        m_screenSize = size;
        OnResize(size);
    }

    virtual void OnResize(const glm::ivec2 &size) = 0;

    [[nodiscard]] const glm::ivec2 &GetSize() const {
        return m_screenSize;
    }

    virtual void SetCameraInfo(const glm::mat4 &view, float fov, float near, float far) = 0;

    virtual void SetWorldLight(
            const glm::vec3 &lightDirection,
            const glm::vec3 &lightColor,
            const glm::vec3 &ambientColor
    ) = 0;

    virtual void SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) = 0;

    virtual void BeginDraw() = 0;

    virtual void EndDraw() = 0;

    void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float range) {
        const float linear = PointLightAttenuationLinearFromRange(range);
        const float quadratic = PointLightAttenuationQuadraticFromRange(range);
        DrawPointLight(position, color, linear, quadratic);
    }

    virtual void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float linear, float quadratic) = 0;

    virtual void DrawLines(const MeshPositionOnly &lines, const glm::vec4 &color) = 0;

    virtual void DrawMesh(const MeshBase &mesh, const glm::mat4 &model, const Material *material) = 0;

private:
    glm::ivec2 m_screenSize{};
};
