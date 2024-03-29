//
// Created by andyroiiid on 11/25/2022.
//
#include "haru/render/ShadowMatrixCalculator.h"

#include <glm/gtc/matrix_transform.hpp>

std::array<glm::vec4, 8> ShadowMatrixCalculator::GetFrustumCorners(float near, float far) const {
    std::array<glm::vec4, 8> corners{
            glm::vec4{-1, -1, -1, 1},
            glm::vec4{-1, -1, 1, 1},
            glm::vec4{-1, 1, -1, 1},
            glm::vec4{-1, 1, 1, 1},
            glm::vec4{1, -1, -1, 1},
            glm::vec4{1, -1, 1, 1},
            glm::vec4{1, 1, -1, 1},
            glm::vec4{1, 1, 1, 1},
    };
    const glm::mat4 inverseCamera = glm::inverse(glm::perspective(m_fov, m_aspectRatio, near, far) * m_view);
    for (glm::vec4 &corner: corners) {
        glm::vec4 cornerWorldPos = inverseCamera * corner;
        corner = cornerWorldPos / cornerWorldPos.w;
    }
    return corners;
}

static glm::vec3 GetFrustumCenter(const std::array<glm::vec4, 8> &corners) {
    glm::vec3 center(0.0f);
    for (const auto &corner: corners) {
        center += glm::vec3(corner);
    }
    center /= corners.size();
    return center;
}

void ShadowMatrixCalculator::SetWorldBounds(const glm::vec3 &min, const glm::vec3 &max) {
    m_worldCorners = {
            glm::vec4{min.x, min.y, min.z, 1},
            glm::vec4{min.x, min.y, max.z, 1},
            glm::vec4{min.x, max.y, min.z, 1},
            glm::vec4{min.x, max.y, max.z, 1},
            glm::vec4{max.x, min.y, min.z, 1},
            glm::vec4{max.x, min.y, max.z, 1},
            glm::vec4{max.x, max.y, min.z, 1},
            glm::vec4{max.x, max.y, max.z, 1},
    };
}

glm::mat4 ShadowMatrixCalculator::GetLightProjection(const std::array<glm::vec4, 8> &frustumCorners, const glm::mat4 &lightView) const {
    constexpr float fMin = std::numeric_limits<float>::min();
    constexpr float fMax = std::numeric_limits<float>::max();

    float minX = fMax;
    float maxX = fMin;
    float minY = fMax;
    float maxY = fMin;
    float zFar = fMax;
    float zNear = fMin;

    for (const glm::vec4 &corner: frustumCorners) {
        const glm::vec4 lightSpaceCorner = lightView * corner;
        minX = glm::min(minX, lightSpaceCorner.x);
        maxX = glm::max(maxX, lightSpaceCorner.x);
        minY = glm::min(minY, lightSpaceCorner.y);
        maxY = glm::max(maxY, lightSpaceCorner.y);
        zFar = glm::min(zFar, lightSpaceCorner.z);
        zNear = glm::max(zNear, lightSpaceCorner.z);
    }

    for (const glm::vec4 &worldCorner: m_worldCorners) {
        const glm::vec4 lightSpaceWorldCorner = lightView * worldCorner;
        zNear = glm::max(zNear, lightSpaceWorldCorner.z);
    }

    return glm::ortho(minX, maxX, minY, maxY, -zNear, -zFar);
}

glm::mat4 ShadowMatrixCalculator::CalcShadowMatrix(float near, float far) const {
    std::array<glm::vec4, 8> frustumCorners = GetFrustumCorners(near, far);
    glm::vec3 center = GetFrustumCenter(frustumCorners);
    static constexpr glm::vec3 UP{0.0f, 1.0f, 0.0f};
    glm::mat4 view = glm::lookAt(center + m_lightDir, center, UP);
    glm::mat4 projection = GetLightProjection(frustumCorners, view);
    return projection * view;
}
