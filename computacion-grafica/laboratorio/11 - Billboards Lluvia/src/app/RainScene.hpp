#pragma once

#include "rain/RainSystem.hpp"
#include "rendering/GroundRenderer.hpp"
#include "rendering/RainRenderer.hpp"
#include "scene/OrbitCamera.hpp"

struct GLFWwindow;

class RainScene
{
public:
    RainScene();

    bool initialize();
    void attachToWindow(GLFWwindow *window);
    void update(float deltaTime);
    void render();
    void updateViewport(int width, int height);

    OrbitCamera &camera();

private:
    OrbitCamera m_camera;
    RainSystem m_rainSystem;
    GroundRenderer m_groundRenderer;
    RainRenderer m_rainRenderer;
    int m_framebufferWidth;
    int m_framebufferHeight;
};
