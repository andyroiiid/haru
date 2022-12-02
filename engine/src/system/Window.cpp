//
// Created by andyroiiid on 11/20/2022.
//

#include "haru/system/Window.h"

#include <glad/gl.h>

#include "haru/core/Debug.h"

static void ErrorCallback(int errorCode, const char *description) {
    DebugLog("GLFW error %d: %s", errorCode, description);
}

static void FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
    const auto app = static_cast<App *>(glfwGetWindowUserPointer(window));
    if (app == nullptr) return;
    app->Resize({width, height});
}

Window::Window() {
    glfwSetErrorCallback(ErrorCallback);

    DebugCheckCritical(glfwInit() == GLFW_TRUE, "Failed to init GLFW");
    DebugLog("GLFW %d.%d.%d initialized", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    DebugCheckCritical(atexit(glfwTerminate) == 0, "Failed to register glfwTerminate");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(1366, 768, "Haru", nullptr, nullptr);
    DebugCheckCritical(m_window != nullptr, "Failed to create window");

    glfwMakeContextCurrent(m_window);
    DebugCheckCritical(gladLoadGL(glfwGetProcAddress) != 0, "Failed to load glad");

    DebugLog("GL_VERSION: %s", glGetString(GL_VERSION));
    DebugLog("GL_SHADING_LANGUAGE_VERSION: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    DebugLog("GL_VENDOR: %s", glGetString(GL_VENDOR));
    DebugLog("GL_RENDERER: %s", glGetString(GL_RENDERER));

    glfwSetFramebufferSizeCallback(m_window, FramebufferSizeCallback);
}

Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Window::MainLoop(App &app) {
    glfwSetWindowUserPointer(m_window, &app);
    app.Window = this;

    app.Init();

    // trigger initial resize
    app.Resize(GetSize());

    glm::vec2 prevMousePos = GetMousePosition();
    double prevTime = glfwGetTime();
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        {
            const glm::vec2 currMousePos = GetMousePosition();
            m_deltaMousePos = currMousePos - prevMousePos;
            prevMousePos = currMousePos;
        }
        {
            const double currentTime = glfwGetTime();
            app.Update(static_cast<float>(currentTime - prevTime));
            prevTime = currentTime;
        }
        app.Draw();
        glfwSwapBuffers(m_window);
    }

    app.Shutdown();

    app.Window = nullptr;
    glfwSetWindowUserPointer(m_window, nullptr);
}

void Window::Close() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

glm::ivec2 Window::GetSize() const {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return {width, height};
}

void Window::SetCursorEnabled(const bool enable) {
    glfwSetInputMode(m_window, GLFW_CURSOR, enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

bool Window::IsKeyDown(const int key) const {
    return glfwGetKey(m_window, key);
}

bool Window::IsMouseButtonDown(int button) const {
    return glfwGetMouseButton(m_window, button);
}

glm::vec2 Window::GetMousePosition() const {
    double xPos, yPos;
    glfwGetCursorPos(m_window, &xPos, &yPos);
    return {xPos, yPos};
}
