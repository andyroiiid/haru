//
// Created by andyroiiid on 11/30/2022.
//

#include "LoadMap.h"

#include <map>
#include <sstream>
#include <foundation/PxVec3.h>
#include <haru/core/Debug.h>
#include <haru/render/MeshBase.h>
#include <haru/system/Files.h>

void LoadEntityProperties(std::stringstream &mapStream, std::map<std::string, std::string> &properties) {
    int numProperties;
    mapStream >> numProperties;
    DebugLog("%d properties", numProperties);

    // read the line end after numProperties
    std::string dummy;
    std::getline(mapStream, dummy);

    for (int i = 0; i < numProperties; i++) {
        std::string key;
        std::getline(mapStream, key);

        std::string value;
        std::getline(mapStream, value);

        DebugLog("%s -> %s", key.c_str(), value.c_str());
    }
}

struct EntityBrushes {
    std::map<std::string, std::vector<VertexBase>> TextureToVertices;
    std::vector<std::vector<physx::PxVec3>> Colliders;
};

void LoadEntityBrushFace(std::stringstream &mapStream, EntityBrushes &brushes) {
    std::string textureName;
    mapStream >> textureName;

    auto &vertices = brushes.TextureToVertices[textureName];

    glm::vec3 normal;
    mapStream >> normal.x >> normal.y >> normal.z;

    int brushFaceVertexCount;
    mapStream >> brushFaceVertexCount;

    std::vector<glm::vec3> brushFaceVertexPositions(brushFaceVertexCount);
    std::vector<glm::vec2> brushFaceVertexTextureCoords(brushFaceVertexCount);
    for (int k = 0; k < brushFaceVertexCount; k++) {
        glm::vec3 &position = brushFaceVertexPositions[k];
        mapStream >> position.x >> position.y >> position.z;
        glm::vec2 &textureCoord = brushFaceVertexTextureCoords[k];
        mapStream >> textureCoord.x >> textureCoord.y;
    }

    // triangulate
    for (int k = 1; k < brushFaceVertexCount - 1; k++) {
        vertices.push_back({brushFaceVertexPositions[0], normal, brushFaceVertexTextureCoords[0]});
        vertices.push_back({brushFaceVertexPositions[k], normal, brushFaceVertexTextureCoords[k]});
        vertices.push_back({brushFaceVertexPositions[k + 1], normal, brushFaceVertexTextureCoords[k + 1]});
    }
}

void LoadEntityBrush(std::stringstream &mapStream, EntityBrushes &brushes, int brushIndex) {
    int numColliderVertices;
    mapStream >> numColliderVertices;

    std::vector<physx::PxVec3> &colliderVertices = brushes.Colliders[brushIndex];
    colliderVertices.reserve(numColliderVertices);
    for (int j = 0; j < numColliderVertices; j++) {
        physx::PxVec3 vertex;
        mapStream >> vertex.x >> vertex.y >> vertex.z;
        colliderVertices.push_back(vertex);
    }

    int numBrushFaces;
    mapStream >> numBrushFaces;
    for (int j = 0; j < numBrushFaces; j++) {
        LoadEntityBrushFace(mapStream, brushes);
    }
}

void LoadEntityBrushes(std::stringstream &mapStream, EntityBrushes &brushes) {
    int numBrushes;
    mapStream >> numBrushes;
    DebugLog("%d brushes", numBrushes);

    brushes.Colliders.resize(numBrushes);

    for (int i = 0; i < numBrushes; i++) {
        LoadEntityBrush(mapStream, brushes, i);
    }
}

void LoadEntity(std::stringstream &mapStream) {
    std::map<std::string, std::string> properties;
    LoadEntityProperties(mapStream, properties);

    EntityBrushes brushes;
    LoadEntityBrushes(mapStream, brushes);
}

void LoadMap(const std::string &mapFilename) {
    std::stringstream mapStream(ReadFile(mapFilename));

    int numEntities;
    mapStream >> numEntities;
    DebugLog("%d entities", numEntities);

    for (int i = 0; i < numEntities; i++) {
        LoadEntity(mapStream);
    }
}
