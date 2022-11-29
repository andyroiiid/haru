//
// Created by andyroiiid on 11/28/2022.
//

#include "MdlLoader.h"

#include <haru/core/Debug.h>
#include <haru/system/Files.h>

#include "MdlStructs.h"

MdlLoader::MdlLoader(const std::string &filename)
        : m_bytes(ReadFile(filename)),
          m_currentPos(m_bytes.data()),
          m_remainingBytes(m_bytes.size()) {
    if (!LoadHeader()) {
        DebugLog("Invalid header");
        return;
    }

    if (!LoadSkins()) {
        DebugLog("Failed to load textures");
        return;
    }

    if (!LoadTextureCoords()) {
        DebugLog("Failed to load texture coords");
        return;
    }

    if (!LoadTriangles()) {
        DebugLog("Failed to load triangles");
        return;
    }

    if (!LoadFrames()) {
        DebugLog("Failed to load frames");
        return;
    }
}

bool MdlLoader::ReadBytes(size_t numBytes, void *output) {
    if (m_remainingBytes < numBytes) {
        // not enough data
        return false;
    }
    memcpy(output, m_currentPos, numBytes);
    m_currentPos += numBytes;
    m_remainingBytes -= numBytes;
    return true;
}

void DebugLogVec3(const char *name, const glm::vec3 &vec3) {
    DebugLog("%s = %f, %f, %f", name, vec3.x, vec3.y, vec3.z);
}

bool MdlLoader::LoadHeader() {
    mdl_t header;

    if (!Read(header)) return false;

    assert(header.ident == (('O' << 24) + ('P' << 16) + ('D' << 8) + 'I'));
    assert(header.version == 6);

    m_scale = header.scale;
    m_origin = header.scale_origin;
    m_radius = header.boundingradius;
    m_eyePosition = header.eyeposition;

    m_numSkins = header.numskins;
    m_skinWidth = header.skinwidth;
    m_skinHeight = header.skinheight;

    m_numVertices = header.numverts;
    m_numTriangles = header.numtris;
    m_numFrames = header.numframes;

    m_syncType = header.synctype;
    m_flags = header.flags;

    return true;
}

bool MdlLoader::LoadSkins() {
    const size_t skinDataSize = m_skinWidth * m_skinHeight;
    for (int i = 0; i < m_numSkins; i++) {
        int32_t group;
        if (!Read(group)) return false;

        if (group == 0) {
            // single skin

            std::vector<uint8_t> skinData(skinDataSize);
            if (!Read(skinData)) return false;
        } else {
            // group skins

            int32_t number;
            if (!Read(number)) return false;

            std::vector<float> time(number);
            if (!Read(time)) return false;

            for (int j = 0; j < number; j++) {
                std::vector<uint8_t> skinData(skinDataSize);
                if (!Read(skinData)) return false;
            }
        }
    }

    return true;
}

bool MdlLoader::LoadTextureCoords() {
    std::vector<stvert_t> textureCoords(m_numVertices);
    if (!Read(textureCoords)) return false;

    return true;
}

bool MdlLoader::LoadTriangles() {
    std::vector<dtriangle_t> triangles(m_numTriangles);
    if (!Read(triangles)) return false;

    return true;
}

bool MdlLoader::LoadFrames() {
    for (int i = 0; i < m_numFrames; i++) {
        int type;
        if (!Read(type)) return false;

        if (type == 0) {
            // one simple frame
            daliasframe_t frame;
            if (!Read(frame)) return false;

            std::vector<trivertx_t> vertices(m_numVertices);
            if (!Read(vertices)) return false;
        } else {
            // group of simple frames
            int numSimpleFrames = type;

            trivertx_t min;
            if (!Read(min)) return false;

            trivertx_t max;
            if (!Read(max)) return false;

            std::vector<float> time(numSimpleFrames);
            if (!Read(time)) return false;

            for (int j = 0; j < numSimpleFrames; j++) {
                daliasframe_t frame;
                if (!Read(frame)) return false;

                std::vector<trivertx_t> vertices(m_numVertices);
                if (!Read(vertices)) return false;
            }
        }
    }

    return true;
}
