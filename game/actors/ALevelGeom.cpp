//
// Created by andyroiiid on 11/21/2022.
//

#include "ALevelGeom.h"

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

ALevelGeom::ALevelGeom(const std::string &levelName) {
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
    for (physx::PxRigidStatic *rigidbody: m_brushRigidbodies) {
        PX_RELEASE(rigidbody)
    }

    for (physx::PxConvexMesh *collider: m_brushColliders) {
        PX_RELEASE(collider);
    }

    m_mesh = {};
}

void ALevelGeom::Update(float deltaTime) {
}

void ALevelGeom::Draw(Renderer &renderer) {
    renderer.DrawMesh(m_mesh, GetTransform().GetMatrix());
}
