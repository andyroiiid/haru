//
// Created by andyroiiid on 11/20/2022.
//

#include <glm/gtc/matrix_transform.hpp>

#include <haru/Window.h>
#include <haru/DeferredRenderer.h>
#include <haru/MeshUtilities.h>

class Game : public App {
public:
    void Init() override {
        m_view = glm::lookAt(
                glm::vec3{3, 4, 5},
                glm::vec3{0, 0, 0},
                glm::vec3{0, 1, 0}
        );

        m_mesh = MeshBase(CreateBox({1.0f, 1.0f, 1.0f}), GL_TRIANGLES);
    }

    void Shutdown() override {
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

        m_renderer.EndDraw();
    }

private:
    DeferredRenderer m_renderer;

    glm::mat4 m_view{1.0f};
    glm::mat4 m_projection{1.0f};

    MeshBase m_mesh;
};

int main() {
    Window window;
    Game game;
    window.MainLoop(game);
}