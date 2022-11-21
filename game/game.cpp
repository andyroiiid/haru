//
// Created by andyroiiid on 11/20/2022.
//

#include <glm/gtc/matrix_transform.hpp>

#include <haru/system/Window.h>
#include <haru/core/Debug.h>
#include <haru/render/DeferredRenderer.h>
#include <haru/render/MeshUtilities.h>

class Game : public App {
public:
    void Init() override {
        DebugLog("Game init");

        m_view = glm::lookAt(
                glm::vec3{3, 4, 5},
                glm::vec3{0, 0, 0},
                glm::vec3{0, 1, 0}
        );

        m_mesh = MeshBase(CreateBox({1.0f, 1.0f, 1.0f}), GL_TRIANGLES);

        m_lines = MeshPositionOnly(
                {
                        {{0.0f, 0.0f, 0.0f}},
                        {{2.0f, 0.0f, 0.0f}},
                        {{0.0f, 0.0f, 0.0f}},
                        {{0.0f, 2.0f, 0.0f}},
                        {{0.0f, 0.0f, 0.0f}},
                        {{0.0f, 0.0f, 2.0f}},
                },
                GL_LINES
        );
    }

    void Shutdown() override {
        DebugLog("Game shutdown");
    }

    void Resize(const glm::ivec2 &size) override {
        m_renderer.Resize(size);

        m_projection = glm::perspective(
                glm::radians(60.0f),
                static_cast<float>(size.x) / static_cast<float>(size.y),
                0.01f,
                100.0f
        );
    }

    void Update(float deltaTime) override {
    }

    void Draw() override {
        m_renderer.BeginDraw();

        m_renderer.SetCameraMatrices(m_view, m_projection);
        m_renderer.SetDirectionalLight({1.0f, 2.0f, 3.0f}, 1.0f);

        m_renderer.DrawMesh(m_mesh, glm::mat4{1.0f});
        m_renderer.DrawLines(m_lines, glm::vec4{1.0f, 0.0f, 0.0f, 0.0f});

        m_renderer.EndDraw();
    }

private:
    DeferredRenderer m_renderer;

    glm::mat4 m_view{1.0f};
    glm::mat4 m_projection{1.0f};

    MeshBase m_mesh;
    MeshPositionOnly m_lines;
};

int main() {
    Window window;
    Game game;
    window.MainLoop(game);
}