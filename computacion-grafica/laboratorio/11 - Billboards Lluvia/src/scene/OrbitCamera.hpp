#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct GLFWwindow;

class OrbitCamera
{
public:
    OrbitCamera();

    void updateFromInput(GLFWwindow *window, float deltaTime);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    glm::vec3 getPosition() const;
    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getUp() const;

private:
    float m_yawDegrees;
    float m_pitchDegrees;
    float m_distance;
};
