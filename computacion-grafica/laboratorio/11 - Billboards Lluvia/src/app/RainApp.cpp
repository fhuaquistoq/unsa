#include "RainApp.hpp"

#include <algorithm>
#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "app/AppConfig.hpp"
#include "app/RainScene.hpp"
#include "utils/DeltaTime.hpp"

namespace
{
    void framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        RainScene *scene = static_cast<RainScene *>(glfwGetWindowUserPointer(window));

        if (scene != nullptr)
        {
            scene->updateViewport(width, height);
        }
    }

    void keyCallback(GLFWwindow *window, int key, int, int action, int)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}

int RainApp::run(GLFWwindow *window)
{
    RainScene scene;

    if (!scene.initialize())
    {
        std::cout << "Failed to initialize rain scene\n";
        return -1;
    }

    scene.attachToWindow(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(appconfig::CLEAR_COLOR.r, appconfig::CLEAR_COLOR.g, appconfig::CLEAR_COLOR.b, 1.0f);

    std::cout << "Controls: Left/Right rotate yaw, Up/Down rotate pitch, Esc exits.\n";

    DeltaTime::reset();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        DeltaTime::update();

        const float deltaTime = std::min(DeltaTime::get(), appconfig::MAX_FRAME_DELTA);
        scene.camera().updateFromInput(window, deltaTime);
        scene.update(deltaTime);
        scene.render();

        glfwSwapBuffers(window);
    }

    glfwSetWindowUserPointer(window, nullptr);
    return 0;
}
