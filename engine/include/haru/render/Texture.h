//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <string>
#include <glad/gl.h>
#include <glm/vec2.hpp>

#include "haru/core/MoveOnly.h"

class Texture {
public:
    MOVE_ONLY(Texture)

    enum class Wrap {
        Repeat,
        Clamp,
        MirrorRepeat
    };

    Texture() = default;

    Texture(
            const glm::ivec2 &size,
            const unsigned char *data,
            Wrap wrap = Wrap::Repeat,
            bool filter = false,
            bool mipmaps = false
    );

    static Texture FromFile(
            const std::string &filename,
            Wrap wrap = Wrap::Repeat,
            bool filter = false,
            bool mipmaps = false
    );

    ~Texture();

    [[nodiscard]] const glm::ivec2 &Size() const { return m_size; }

    void Bind(GLuint unit) const;

private:
    MoveOnly<glm::ivec2> m_size;
    MoveOnly<GLuint> m_texture;
};
