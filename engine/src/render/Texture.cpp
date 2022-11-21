//
// Created by andyroiiid on 11/20/2022.
//

#include "haru/render/Texture.h"

#include "haru/core/Debug.h"
#include "haru/system/ImageFile.h"

Texture::Texture(const glm::ivec2 &size, const unsigned char *data, Wrap wrap, bool filter, bool mipmaps) {
    m_size = size;

    glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
    glTextureStorage2D(m_texture, 1, GL_RGBA8, size.x, size.y);
    glTextureSubImage2D(m_texture, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);

    if (filter) {
        glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    switch (wrap) {
    case Wrap::Repeat:
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    case Wrap::Clamp:
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    case Wrap::MirrorRepeat:
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        break;
    case Wrap::MirrorClamp:
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
        glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);
        break;
    }

    if (mipmaps) {
        glGenerateTextureMipmap(m_texture);
    }
}

std::unique_ptr<Texture> Texture::FromFile(const std::string &filename, Texture::Wrap wrap, bool filter, bool mipmaps) {
    ImageFile image(filename);
    if (!image.Data()) {
        DebugLog("Failed to load texture from file: %s", filename.c_str());
        return {};
    }
    DebugLog("Loaded texture from file: %s", filename.c_str());
    return std::make_unique<Texture>(image.Size(), image.Data(), wrap, filter, mipmaps);
}

Texture::~Texture() {
    if (m_texture) glDeleteTextures(1, &m_texture);
}

void Texture::Bind(GLuint unit) const {
    glBindTextureUnit(unit, m_texture);
}