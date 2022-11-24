//
// Created by andyroiiid on 11/21/2022.
//

#include "ALevelGeom.h"

#include <sstream>

#include <haru/core/Debug.h>
#include <haru/system/Files.h>

ALevelGeom::ALevelGeom(const std::string &levelName) {
    DebugLog("loading level %s", levelName.c_str());

    std::vector<VertexBase> vertices;

    std::string map = ReadFile(levelName);
    std::stringstream mapStream(map);

    int brushCount;
    mapStream >> brushCount;
    for (int i = 0; i < brushCount; i++) {
        int brushVertexCount;
        mapStream >> brushVertexCount;

        std::vector<glm::vec3> brushVertices;
        brushVertices.reserve(brushVertexCount);
        for (int j = 0; j < brushVertexCount; j++) {
            glm::vec3 vertex;
            mapStream >> vertex.x >> vertex.y >> vertex.z;
            brushVertices.push_back(vertex);
        }

        int brushFaceCount;
        mapStream >> brushFaceCount;
        for (int j = 0; j < brushFaceCount; j++) {
            glm::vec3 normal;
            mapStream >> normal.x >> normal.y >> normal.z;

            int brushFaceVertexCount;
            mapStream >> brushFaceVertexCount;

            std::vector<glm::vec3> brushFaceVertices(brushFaceVertexCount);
            for (int k = 0; k < brushFaceVertexCount; k++) {
                glm::vec3 &vertex = brushFaceVertices[k];
                mapStream >> vertex.x >> vertex.y >> vertex.z;
            }

            // triangulate
            for (int k = 1; k < brushFaceVertexCount - 1; k++) {
                vertices.push_back({brushFaceVertices[0], normal, {0.0f, 0.0f}});
                vertices.push_back({brushFaceVertices[k], normal, {0.0f, 0.0f}});
                vertices.push_back({brushFaceVertices[k + 1], normal, {0.0f, 0.0f}});
            }
        }
    }

    m_mesh = MeshBase(vertices, GL_TRIANGLES);
}

ALevelGeom::~ALevelGeom() {
    m_mesh = {};
}

void ALevelGeom::Update(float deltaTime) {
}

void ALevelGeom::Draw(Renderer &renderer) {
    renderer.DrawMesh(m_mesh, GetTransform().GetMatrix());
}
