//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <glad/gl.h>

#include "haru/core/MoveOnly.h"

template<class T>
class UniformBuffer {
public:
    MOVE_ONLY(UniformBuffer)

    UniformBuffer() {
        static constexpr GLbitfield MAPPING_ACCESS = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

        glCreateBuffers(1, &m_buffer);
        glNamedBufferStorage(m_buffer, sizeof(T), nullptr, MAPPING_ACCESS);
        void *data = glMapNamedBufferRange(m_buffer, 0, sizeof(T), MAPPING_ACCESS);
        m_data = static_cast<T *>(data);
    }

    ~UniformBuffer() {
        glUnmapNamedBuffer(m_buffer);
        glDeleteBuffers(1, &m_buffer);
    }

    void Bind(const GLuint index) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, m_buffer);
    }

    void Flush() {
        glFlushMappedNamedBufferRange(m_buffer, 0, sizeof(T));
    }

    T *operator->() {
        return m_data;
    }

private:
    MoveOnly<GLuint> m_buffer;
    MoveOnly<T *> m_data;
};