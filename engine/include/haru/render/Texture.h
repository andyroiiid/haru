//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <string>
#include <memory>
#include <glad/gl.h>
#include <glm/vec2.hpp>

#include "haru/core/MoveOnly.h"

class Texture {
public:
    enum class Wrap {
        Repeat,
        Clamp,
        MirrorRepeat,
        MirrorClamp
    };

    Texture() = default;

    Texture(Texture &&) noexcept = default;

    Texture &operator=(Texture &&) noexcept = default;

    Texture(
            const glm::ivec2 &size,
            const unsigned char *data,
            Wrap wrap = Wrap::Repeat,
            bool filter = false,
            bool mipmaps = false
    );

    static std::unique_ptr<Texture> FromFile(
            const std::string &filename,
            Wrap wrap = Wrap::Repeat,
            bool filter = false,
            bool mipmaps = false
    );

    ~Texture();

    [[nodiscard]] inline const glm::ivec2 &Size() const { return m_size; }

    void Bind(GLuint unit) const;

private:
    MoveOnly<glm::ivec2> m_size;
    MoveOnly<GLuint> m_texture;
};