//
// Created by andyroiiid on 11/21/2022.
//

#include "ALevelGeometry.h"

#include <sstream>
#include <PxRigidStatic.h>
#include <foundation/PxAllocator.h>
#include <foundation/PxTransform.h>
#include <geometry/PxConvexMeshGeometry.h>

#include <haru/core/Debug.h>
#include <haru/physics/PhysicsScene.h>
#include <haru/physics/PhysicsSystem.h>
#include <haru/system/Files.h>

#include "../GameStatics.h"

ALevelGeometry::ALevelGeometry(const std::string &levelName) {
    DebugLog("loading level %s", levelName.c_str());

    std::vector<VertexBase> vertices;

    std::string map = ReadFile(levelName);
    std::stringstream mapStream(map);

    const physx::PxTransform rigidbodyTransform(physx::PxVec3{0.0f, 0.0f, 0.0f});
    PhysicsSystem *physicsSystem = GameStatics::GetPhysicsSystem();
    PhysicsScene *physicsScene = GameStatics::GetPhysicsScene();

    int brushCount;
    mapStream >> brushCount;
    for (int i = 0; i < brushCount; i++) {
        int brushVertexCount;
        mapStream >> brushVertexCount;

        std::vector<physx::PxVec3> brushVertices;
        brushVertices.reserve(brushVertexCount);
        for (int j = 0; j < brushVertexCount; j++) {
            physx::PxVec3 vertex;
            mapStream >> vertex.x >> vertex.y >> vertex.z;

            m_geomMin.x = glm::min(vertex.x, m_geomMin.x);
            m_geomMin.y = glm::min(vertex.y, m_geomMin.y);
            m_geomMin.z = glm::min(vertex.z, m_geomMin.z);
            m_geomMax.x = glm::max(vertex.x, m_geomMax.x);
            m_geomMax.y = glm::max(vertex.y, m_geomMax.y);
            m_geomMax.z = glm::max(vertex.z, m_geomMax.z);

            brushVertices.push_back(vertex);
        }

        physx::PxConvexMesh *brushCollider = physicsSystem->CreateConvexMesh(brushVertexCount, brushVertices.data());
        m_brushColliders.push_back(brushCollider);

        physx::PxRigidStatic *brushRigidbody = physicsScene->CreateStatic(rigidbodyTransform, physx::PxConvexMeshGeometry(brushCollider));
        brushRigidbody->userData = this;
        m_brushRigidbodies.push_back(brushRigidbody);

        int brushFaceCount;
        mapStream >> brushFaceCount;
        for (int j = 0; j < brushFaceCount; j++) {
            std::string texture;
            mapStream >> texture;

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
    }

    m_mesh = MeshBase(vertices, GL_TRIANGLES);
}

ALevelGeometry::~ALevelGeometry() {
    for (physx::PxRigidStatic *rigidbody: m_brushRigidbodies) {
        PX_RELEASE(rigidbody)
    }

    for (physx::PxConvexMesh *collider: m_brushColliders) {
        PX_RELEASE(collider);
    }

    m_mesh = {};
}

void ALevelGeometry::Draw(Renderer &renderer) {
    renderer.SetWorldBounds(m_geomMin, m_geomMax);
    renderer.DrawMesh(m_mesh, GetTransform().GetMatrix(), nullptr);
}
