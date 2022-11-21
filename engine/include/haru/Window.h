//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "haru/MoveOnly.h"

class Window;

struct App {
    MOVE_ONLY(App)

    App() = default;

    virtual ~App() = default;

    virtual void Init() = 0;

    virtual void Shutdown() = 0;

    virtual void Resize(const glm::ivec2 &size) = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void Draw() = 0;

    Window *Window = nullptr;
};

class Window {
public:
    MOVE_ONLY(Window)

    Window();

    ~Window();

    void MainLoop(App &app);

    void SetCursorEnabled(bool enable);

    void Close();

    [[nodiscard]] glm::ivec2 GetSize() const;

    [[nodiscard]] bool IsKeyDown(int key) const;

    [[nodiscard]] bool IsMouseButtonDown(int button) const;

    [[nodiscard]] glm::vec2 GetMousePosition() const;

    [[nodiscard]] float GetKeyAxis(int posKey, int negKey) const;

private:
    MoveOnly<GLFWwindow *> m_window;
};