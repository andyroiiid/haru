//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "haru/render/MeshPositionOnly.h"
#include "haru/render/MeshBase.h"

struct Renderer {
    MOVE_ONLY(Renderer)

    Renderer() = default;

    virtual ~Renderer() = default;

    virtual void Resize(const glm::ivec2 &size) = 0;

    virtual void SetCameraMatrices(const glm::mat4 &view, const glm::mat4 &projection) = 0;

    virtual void SetDirectionalLight(const glm::vec3 &lightDirection, float intensity) = 0;

    virtual void BeginDraw() = 0;

    virtual void EndDraw() = 0;

    virtual void DrawPointLight(const glm::vec3 &position, const glm::vec3 &color, float linear, float quadratic) = 0;

    virtual void DrawLines(const MeshPositionOnly &lines, const glm::vec4 &color) = 0;

    virtual void DrawMesh(const MeshBase &mesh, const glm::mat4 &model) = 0;
};