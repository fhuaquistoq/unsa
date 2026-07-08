#pragma once

struct GLFWwindow;

struct OpenGLInitResult
{
    GLFWwindow *window = nullptr;
    int gladVersion = 0;
    const char *errorMessage = nullptr;

    bool ok() const
    {
        return window != nullptr && gladVersion != 0;
    }
};

OpenGLInitResult initializeOpenGL(int width, int height, const char *title);
void shutdownOpenGL(GLFWwindow *window);
