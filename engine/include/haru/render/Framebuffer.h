//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <array>
#include <glad/gl.h>
#include <glm/vec2.hpp>

#include "haru/core/MoveOnly.h"

class Framebuffer {
public:
    MOVE_ONLY(Framebuffer)

    Framebuffer() = default;

    Framebuffer(const glm::ivec2 &size, const std::initializer_list<GLenum> &formats, bool depthIsTexture = false);

    ~Framebuffer();

    [[nodiscard]] const glm::ivec2 &Size() const { return m_size; }

    void Bind();

    static void BindDefault();

    void Clear();

    void BindColorTexture(GLuint unit, GLuint attachment);

    void BindDepthStencilTexture(GLuint unit);

    void BindAllTextures();

    void UnbindAllTextures();

    [[nodiscard]] int GetTextureCount() const {
        return m_depthIsTexture ? m_numColorAttachments + 1 : m_numColorAttachments;
    }

    void BlitDepthStencilToScreen(const glm::ivec2 &screenSize);

private:
    void CreateColorAttachment(GLenum format);

    void CreateColorAttachments(const std::initializer_list<GLenum> &formats);

    void CreateDepthStencilAttachment();

    MoveOnly<glm::ivec2> m_size;
    MoveOnly<GLuint> m_fbo;

    static constexpr int MAX_NUM_COLOR_ATTACHMENTS = 32;
    using ColorAttachmentsArray = std::array<MoveOnly<GLuint>, MAX_NUM_COLOR_ATTACHMENTS>;

    MoveOnly<GLsizei> m_numColorAttachments;
    ColorAttachmentsArray m_colorAttachments;

    MoveOnly<bool> m_depthIsTexture;
    MoveOnly<GLuint> m_depthStencilAttachment;
};
