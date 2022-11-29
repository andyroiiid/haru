//
// Created by andyroiiid on 11/28/2022.
//

#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>

#include <haru/render/Texture.h>

class MdlLoader {
public:
    explicit MdlLoader(const std::string &filename);

private:
    bool ReadBytes(size_t numBytes, void *output);

    template<typename T>
    bool Read(std::vector<T> &vector) {
        return ReadBytes(vector.size() * sizeof(T), vector.data());
    }

    template<typename T>
    bool Read(T &value) { return ReadBytes(sizeof(T), &value); }

    bool LoadHeader();

    bool LoadSkins();

    bool LoadTextureCoords();

    bool LoadTriangles();

    bool LoadFrames();

    std::string m_bytes;
    const char *m_currentPos;
    size_t m_remainingBytes;

    glm::vec3 m_scale{};
    glm::vec3 m_origin{};
    float m_radius{};
    glm::vec3 m_eyePosition{};
    long m_numSkins{};
    long m_skinWidth{};
    long m_skinHeight{};
    long m_numVertices{};
    long m_numTriangles{};
    long m_numFrames{};
    long m_syncType{};
    long m_flags{};
    float m_size{};
};
