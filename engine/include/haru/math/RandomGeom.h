//
// Created by andyroiiid on 11/24/2022.
//

#pragma once

#include <cmath>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>

#include "haru/math/Random.h"

class RandomGeom : Random {
public:
    explicit RandomGeom(uint64_t seed) : Random(seed) {}

    float Angle() {
        return 2.0f * PI * Float();
    }

    glm::vec2 OnCircle() {
        float theta = Angle();
        return {glm::cos(theta), glm::sin(theta)};
    }

    glm::vec2 InCircle() {
        float r = std::sqrt(Float());
        return OnCircle() * r;
    }

    glm::vec3 OnSphere() {
        float theta = Angle();
        float phi = std::acos(Float(-1.0f, 1.0f));
        return {
                glm::sin(phi) * glm::cos(theta),
                glm::sin(phi) * glm::sin(theta),
                glm::cos(phi)
        };
    }

    glm::vec3 InSphere() {
        float r = std::cbrt(Float());
        return OnSphere() * r;
    }

    glm::vec3 InHemisphere(const glm::vec3 &normal) {
        glm::vec3 v = InSphere();
        return glm::dot(v, normal) > 0.0f ? v : -v;
    }

private:
    static constexpr float PI = 3.14159265358979323846f;
};
