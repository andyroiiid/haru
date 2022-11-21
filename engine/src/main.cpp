#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(800, 600, "haru", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        constexpr glm::vec4 clearColor{0.4f, 0.8f, 1.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, glm::value_ptr(clearColor));
        glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.0f, 0);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}