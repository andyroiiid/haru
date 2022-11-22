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

    int facesLoaded = 0;

    std::string map = ReadFile(levelName);
    std::stringstream mapStream(map);
    while (!mapStream.eof()) {
        glm::vec3 normal;
        mapStream >> normal.x >> normal.y >> normal.z;

        int count;
        mapStream >> count;

        std::vector<glm::vec3> faceVertices;
        for (int i = 0; i < count; i++) {
            glm::vec3 vertex;
            mapStream >> vertex.x >> vertex.y >> vertex.z;
            faceVertices.push_back(vertex / 32.0f); // scale 1/32
        }

        for (int i = 1; i < count - 1; i++) {
            vertices.push_back({faceVertices[0], normal, {0.0f, 0.0f}});
            vertices.push_back({faceVertices[i], normal, {0.0f, 0.0f}});
            vertices.push_back({faceVertices[i + 1], normal, {0.0f, 0.0f}});
        }

        facesLoaded++;
    }

    DebugLog("%d faces loaded", facesLoaded);

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
