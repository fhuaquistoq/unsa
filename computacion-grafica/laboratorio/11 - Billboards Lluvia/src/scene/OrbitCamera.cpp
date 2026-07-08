#include "OrbitCamera.hpp"

#include <algorithm>

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "app/AppConfig.hpp"

OrbitCamera::OrbitCamera()
    : m_yawDegrees(appconfig::camera::INITIAL_YAW_DEGREES),
      m_pitchDegrees(appconfig::camera::INITIAL_PITCH_DEGREES),
      m_distance(appconfig::camera::DISTANCE)
{
}

void OrbitCamera::updateFromInput(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        m_yawDegrees -= appconfig::camera::ROTATION_SPEED * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        m_yawDegrees += appconfig::camera::ROTATION_SPEED * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        m_pitchDegrees += appconfig::camera::ROTATION_SPEED * deltaTime;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        m_pitchDegrees -= appconfig::camera::ROTATION_SPEED * deltaTime;
    }

    m_pitchDegrees = std::clamp(m_pitchDegrees,
                                appconfig::camera::MIN_PITCH,
                                appconfig::camera::MAX_PITCH);
}

glm::mat4 OrbitCamera::getViewMatrix() const
{
    return glm::lookAt(getPosition(), appconfig::camera::LOOK_TARGET, appconfig::WORLD_UP);
}

glm::mat4 OrbitCamera::getProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(appconfig::camera::FIELD_OF_VIEW_DEGREES),
                            aspectRatio,
                            appconfig::camera::NEAR_PLANE,
                            appconfig::camera::FAR_PLANE);
}

glm::vec3 OrbitCamera::getPosition() const
{
    const float yawRadians = glm::radians(m_yawDegrees);
    const float pitchRadians = glm::radians(m_pitchDegrees);

    const float horizontalDistance = m_distance * glm::cos(pitchRadians);
    const float verticalDistance = m_distance * glm::sin(pitchRadians);

    return glm::vec3(
               appconfig::camera::LOOK_TARGET.x + horizontalDistance * glm::sin(yawRadians),
               appconfig::camera::LOOK_TARGET.y + verticalDistance,
               appconfig::camera::LOOK_TARGET.z + horizontalDistance * glm::cos(yawRadians));
}

glm::vec3 OrbitCamera::getForward() const
{
    return glm::normalize(appconfig::camera::LOOK_TARGET - getPosition());
}

glm::vec3 OrbitCamera::getRight() const
{
    return glm::normalize(glm::cross(getForward(), appconfig::WORLD_UP));
}

glm::vec3 OrbitCamera::getUp() const
{
    return glm::normalize(glm::cross(getRight(), getForward()));
}
