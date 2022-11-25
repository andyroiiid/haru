//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <string>
#include <glad/gl.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "haru/core/MoveOnly.h"

class Shader {
public:
    MOVE_ONLY(Shader)

    Shader() = default;

    Shader(
            const std::string &sharedSource,
            const std::string &vertexSource,
            const std::string &fragmentSource
    );

    Shader(
            const std::string &sharedSource,
            const std::string &vertexSource,
            const std::string &geometrySource,
            const std::string &fragmentSource
    );

    ~Shader();

    void Use() const;

    [[nodiscard]] GLint GetUniformLocation(const std::string &name) const;

    void SetUniform(GLint location, float value);

    void SetUniform(GLint location, const glm::vec2 &value);

    void SetUniform(GLint location, const glm::vec3 &value);

    void SetUniform(GLint location, const glm::vec4 &value);

    void SetUniform(GLint location, const glm::mat4 &value);

private:
    MoveOnly<GLuint> m_program;
};