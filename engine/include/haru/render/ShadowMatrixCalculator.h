//
// Created by andyroiiid on 11/25/2022.
//

#pragma once

#include <array>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class ShadowMatrixCalculator {
public:
    void SetCameraInfo(const glm::mat4 &view, float fov, float aspectRatio) {
        m_view = view;
        m_fov = fov;
        m_aspectRatio = aspectRatio;
    }

    [[nodiscard]] glm::mat4 CalcShadowMatrix(const glm::vec3 &lightDir, float near, float far) const;

private:
    [[nodiscard]] std::array<glm::vec4, 8> GetFrustumCorners(float near, float far) const;

    glm::mat4 m_view{1.0f};
    float m_fov = 0.0f;
    float m_aspectRatio = 1.0f;
};
