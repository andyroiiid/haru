//
// Created by andyroiiid on 11/20/2022.
//

#include "haru/render/Shader.h"

#include <vector>
#include <glm/gtc/type_ptr.hpp>

#include "haru/core/Debug.h"

static GLuint CreateShader(GLenum type, const std::vector<const char *> &&source) {
    const GLuint shader = glCreateShader(type);

    glShaderSource(shader, static_cast<GLsizei>(source.size()), source.data(), nullptr);
    glCompileShader(shader);

    GLint compileStatus = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        GLint infoLogLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        const auto infoLog = new GLchar[infoLogLength];
        glGetShaderInfoLog(shader, infoLogLength, &infoLogLength, infoLog);
        DebugLog("Failed to load shader: %s", infoLog);
        delete[] infoLog;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static GLuint CreateProgram(const std::initializer_list<GLuint> &shaders) {
    const GLuint program = glCreateProgram();

    for (const GLuint shader: shaders)
        glAttachShader(program, shader);
    glLinkProgram(program);

    GLint linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLint infoLogLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        const auto infoLog = new GLchar[infoLogLength];
        glGetProgramInfoLog(program, infoLogLength, &infoLogLength, infoLog);
        DebugLog("Failed to link program: %s", infoLog);
        delete[] infoLog;
        glDeleteProgram(program);
        return 0;
    }

    for (const GLuint shader: shaders)
        glDetachShader(program, shader);
    return program;
}

Shader::Shader(
        const std::string &sharedSource,
        const std::string &vertexSource,
        const std::string &fragmentSource
) {
    auto versionString = "#version 450 core\n";
    GLuint vertexShader = CreateShader(
            GL_VERTEX_SHADER,
            {
                    versionString,
                    sharedSource.c_str(),
                    vertexSource.c_str()
            }
    );
    GLuint fragmentShader = CreateShader(
            GL_FRAGMENT_SHADER,
            {
                    versionString,
                    sharedSource.c_str(),
                    fragmentSource.c_str()
            }
    );

    m_program = CreateProgram({vertexShader, fragmentShader});

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(
        const std::string &sharedSource,
        const std::string &vertexSource,
        const std::string &geometrySource,
        const std::string &fragmentSource
) {
    auto versionString = "#version 450 core\n";
    GLuint vertexShader = CreateShader(
            GL_VERTEX_SHADER,
            {
                    versionString,
                    sharedSource.c_str(),
                    vertexSource.c_str()
            }
    );
    GLuint geometryShader = CreateShader(
            GL_GEOMETRY_SHADER,
            {
                    versionString,
                    sharedSource.c_str(),
                    geometrySource.c_str()
            }
    );
    GLuint fragmentShader = CreateShader(
            GL_FRAGMENT_SHADER,
            {
                    versionString,
                    sharedSource.c_str(),
                    fragmentSource.c_str()
            }
    );

    m_program = CreateProgram({vertexShader, geometryShader, fragmentShader});

    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    if (m_program)
        glDeleteProgram(m_program);
}

void Shader::Use() const {
    glUseProgram(m_program);
}

GLint Shader::GetUniformLocation(const std::string &name) const {
    return glGetUniformLocation(m_program, name.c_str());
}

void Shader::SetUniform(const GLint location, const float value) {
    glProgramUniform1f(m_program, location, value);
}

void Shader::SetUniform(const GLint location, const glm::vec2 &value) {
    glProgramUniform2fv(m_program, location, 1, glm::value_ptr(value));
}

void Shader::SetUniform(const GLint location, const glm::vec3 &value) {
    glProgramUniform3fv(m_program, location, 1, glm::value_ptr(value));
}

void Shader::SetUniform(const GLint location, const glm::vec4 &value) {
    glProgramUniform4fv(m_program, location, 1, glm::value_ptr(value));
}

void Shader::SetUniform(GLint location, const glm::mat4 &value) {
    glProgramUniformMatrix4fv(m_program, location, 1, GL_FALSE, glm::value_ptr(value));
}