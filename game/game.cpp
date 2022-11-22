//
// Created by andyroiiid on 11/20/2022.
//

#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

#include <haru/core/Debug.h>
#include <haru/math/Transform.h>
#include <haru/physics/PhysicsSystem.h>
#include <haru/render/DeferredRenderer.h>
#include <haru/render/MeshUtilities.h>
#include <haru/system/Files.h>
#include <haru/system/Window.h>

struct CameraConfig {
    float MouseSpeed = 0.001f;
    float FlySpeed = 128.0f;

    float FieldOfViewDegrees = 90.0f;
    float NearPlane = 1.0f;
    float FarPlane = 1000.0f;
};

class Game : public App {
public:
    void Init() override {
        DebugLog("Game init");

        Window->SetCursorEnabled(false);
        m_cameraTransform
                .SetPosition({128.0f, 128.0f, 128.0f})
                .RotateY(glm::radians(135.0f))
                .RotateX(glm::radians(-30.0f));

        {
            std::vector<VertexBase> vertices;

            std::string map = ReadFile("data/hello.haru");
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
                    faceVertices.push_back(vertex);
                }

                for (int i = 1; i < count - 1; i++) {
                    vertices.push_back({faceVertices[0], normal, {0.0f, 0.0f}});
                    vertices.push_back({faceVertices[i], normal, {0.0f, 0.0f}});
                    vertices.push_back({faceVertices[i + 1], normal, {0.0f, 0.0f}});
                }
            }

            m_mesh = MeshBase(vertices, GL_TRIANGLES);
        }

        m_lines = MeshPositionOnly(
                {
                        {{0.0f,   0.0f,   0.0f}},
                        {{100.0f, 0.0f,   0.0f}},
                        {{0.0f,   0.0f,   0.0f}},
                        {{0.0f,   100.0f, 0.0f}},
                        {{0.0f,   0.0f,   0.0f}},
                        {{0.0f,   0.0f,   100.0f}},
                },
                GL_LINES
        );
    }

    void Shutdown() override {
        DebugLog("Game shutdown");
    }

    void Resize(const glm::ivec2 &size) override {
        m_renderer.Resize(size);
    }

    void Update(float deltaTime) override {
        m_physics.Update(deltaTime);

        if (Window->IsKeyDown(GLFW_KEY_ESCAPE)) {
            Window->Close();
            return;
        }

        Transform &transform = m_cameraTransform;

        const glm::vec2 currMousePos = Window->GetMousePosition();
        const glm::vec2 deltaMousePos = currMousePos - m_prevMousePos;
        transform
                .RotateX(m_cameraConfig.MouseSpeed * -deltaMousePos.y)
                .RotateY(m_cameraConfig.MouseSpeed * -deltaMousePos.x)
                .ClampPitch();
        m_prevMousePos = currMousePos;

        static constexpr glm::vec3 UP{0.0f, 1.0f, 0.0f};
        if (const glm::vec3 inputDirection =
                    transform.GetHorizontalRightVector() * Window->GetKeyAxis(GLFW_KEY_D, GLFW_KEY_A) +
                    UP * Window->GetKeyAxis(GLFW_KEY_SPACE, GLFW_KEY_LEFT_CONTROL) +
                    transform.GetHorizontalForwardVector() * Window->GetKeyAxis(GLFW_KEY_W, GLFW_KEY_S);
                inputDirection.x != 0.0f || inputDirection.y != 0.0f || inputDirection.z != 0.0f) {
            float flySpeed = m_cameraConfig.FlySpeed;
            if (Window->IsKeyDown(GLFW_KEY_LEFT_SHIFT)) {
                flySpeed *= 2.0f;
            }
            transform.Translate(normalize(inputDirection) * (flySpeed * deltaTime));
        }
    }

    void Draw() override {
        m_renderer.BeginDraw();

        const glm::ivec2 screenSize = Window->GetSize();
        m_renderer.SetCameraMatrices(
                m_cameraTransform.GetInverseMatrix(),
                glm::perspective(
                        glm::radians(m_cameraConfig.FieldOfViewDegrees),
                        static_cast<float>(screenSize.x) / static_cast<float>(screenSize.y),
                        m_cameraConfig.NearPlane,
                        m_cameraConfig.FarPlane
                )
        );

        m_renderer.SetDirectionalLight({1.0f, 2.0f, 3.0f}, 1.0f);

        m_renderer.DrawMesh(m_mesh, glm::mat4{1.0f});
        m_renderer.DrawLines(m_lines, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});

        m_renderer.DrawPointLight(m_cameraTransform.GetPosition(), {1.0f, 1.0f, 1.0f}, 0.014f, 0.0007f);

        m_renderer.EndDraw();
    }

private:
    DeferredRenderer m_renderer;
    PhysicsSystem m_physics;

    MeshBase m_mesh;
    MeshPositionOnly m_lines;

    Transform m_cameraTransform;
    CameraConfig m_cameraConfig{};
    glm::vec2 m_prevMousePos{};
};

int main() {
    Window window;
    Game game;
    window.MainLoop(game);
}