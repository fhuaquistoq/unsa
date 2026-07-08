#include <GLFW/glfw3.h>

#include "DeltaTime.hpp"

float DeltaTime::lastTime = 0.0f;
float DeltaTime::deltaTime = 0.0f;

float DeltaTime::get()
{
    return deltaTime;
}

void DeltaTime::reset()
{
    lastTime = static_cast<float>(glfwGetTime());
    deltaTime = 0.0f;
}

void DeltaTime::update()
{
    float currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
}
