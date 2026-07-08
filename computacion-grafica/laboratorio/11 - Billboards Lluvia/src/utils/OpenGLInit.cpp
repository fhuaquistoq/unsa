#include "OpenGLInit.hpp"

#define GLFW_INCLUDE_NONE

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace
{
    constexpr int OPENGL_VERSION_MAJOR = 3;
    constexpr int OPENGL_VERSION_MINOR = 3;
}

OpenGLInitResult initializeOpenGL(int width, int height, const char *title)
{
    if (!glfwInit())
    {
        OpenGLInitResult result;
        result.errorMessage = "Failed to initialize GLFW";
        return result;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        OpenGLInitResult result;
        result.errorMessage = "Failed to create GLFW window";
        return result;
    }

    glfwMakeContextCurrent(window);

    int gladVersion = gladLoadGL(glfwGetProcAddress);

    if (gladVersion == 0)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        OpenGLInitResult result;
        result.errorMessage = "Failed to load OpenGL with GLAD";
        return result;
    }

    OpenGLInitResult result;
    result.window = window;
    result.gladVersion = gladVersion;
    return result;
}

void shutdownOpenGL(GLFWwindow *window)
{
    if (window != nullptr)
    {
        glfwDestroyWindow(window);
    }

    glfwTerminate();
}
