//
// Created by andyroiiid on 11/20/2022.
//

#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "haru/core/MoveOnly.h"

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

    Window(const std::string &name, const glm::ivec2 &size, bool maximized);

    ~Window();

    void MainLoop(App &app);

    void Close();

    [[nodiscard]] glm::ivec2 GetSize() const;

    void SetCursorEnabled(bool enable);

    [[nodiscard]] bool IsKeyDown(int key) const;

    [[nodiscard]] bool IsMouseButtonDown(int button) const;

    [[nodiscard]] glm::vec2 GetMousePosition() const;

    [[nodiscard]] const glm::vec2 &GetMouseDeltaPosition() const {
        return m_deltaMousePos;
    }

    [[nodiscard]] float GetKeyAxis(int posKey, int negKey) const {
        float value = 0.0f;
        if (IsKeyDown(posKey)) value += 1.0f;
        if (IsKeyDown(negKey)) value -= 1.0f;
        return value;
    }

private:
    MoveOnly<GLFWwindow *> m_window;

    glm::vec2 m_deltaMousePos{};
};
