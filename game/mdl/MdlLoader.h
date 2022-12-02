//
// Created by andyroiiid on 11/28/2022.
//

#pragma once

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <foundation/PxVec3.h>

#include <haru/render/Texture.h>
#include <haru/render/MeshBase.h>

#include "MdlStructs.h"

class MdlLoader {
public:
    explicit MdlLoader(const std::string &filename);

    [[nodiscard]] std::vector<physx::PxVec3> GetColliderTriangles();

    [[nodiscard]] Texture GetTexture() const {
        return {{m_skinWidth, m_skinHeight}, m_textureData.data()};
    }

    [[nodiscard]] MeshBase GetMesh() const {
        return {m_meshVertices, GL_TRIANGLES};
    }

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

    bool LoadSimpleFrame();

    VertexBase GenerateVertex(const std::vector<trivertx_t> &vertices, const dtriangle_t &triangle, int indexInTriangle);

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

    std::vector<stvert_t> m_textureCoords;
    std::vector<dtriangle_t> m_triangles;

    // one skin + one frame (temporary)
    std::vector<unsigned char> m_textureData;
    std::vector<VertexBase> m_meshVertices;
};
