#include "RainScene.hpp"

#include <algorithm>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "app/AppConfig.hpp"

RainScene::RainScene()
    : m_framebufferWidth(appconfig::WINDOW_WIDTH),
      m_framebufferHeight(appconfig::WINDOW_HEIGHT)
{
}

bool RainScene::initialize()
{
    return m_groundRenderer.initialize() &&
           m_rainRenderer.initialize() &&
           m_rainSystem.initialize(appconfig::RAIN_DROP_COUNT);
}

void RainScene::attachToWindow(GLFWwindow *window)
{
    glfwSetWindowUserPointer(window, this);

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    updateViewport(framebufferWidth, framebufferHeight);
}

void RainScene::update(float deltaTime)
{
    m_rainSystem.update(deltaTime);
}

void RainScene::render()
{
    const float aspectRatio = static_cast<float>(m_framebufferWidth) /
                              static_cast<float>(std::max(m_framebufferHeight, 1));

    const glm::mat4 view = m_camera.getViewMatrix();
    const glm::mat4 projection = m_camera.getProjectionMatrix(aspectRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_groundRenderer.render(view, projection);
    m_rainRenderer.render(m_rainSystem, m_camera, view, projection);
}

void RainScene::updateViewport(int width, int height)
{
    m_framebufferWidth = std::max(width, 1);
    m_framebufferHeight = std::max(height, 1);
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);
}

OrbitCamera &RainScene::camera()
{
    return m_camera;
}
