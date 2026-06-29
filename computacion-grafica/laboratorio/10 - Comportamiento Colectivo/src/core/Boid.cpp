#include "Boid.hpp"

#include <glm/geometric.hpp>

namespace
{
    constexpr glm::vec2 FALLBACK_DIRECTION{1.0f, 0.0f};

    glm::vec2 directionFromVelocity(const glm::vec2 &velocity)
    {
        const float speed = glm::length(velocity);

        if (speed <= 0.0001f)
        {
            return FALLBACK_DIRECTION;
        }

        return velocity / speed;
    }
}

Boid::Boid(int id, const glm::vec2 &position, const glm::vec2 &velocity)
{
    this->m_id = id;
    this->m_position = position;
    this->m_velocity = velocity;
    this->m_direction = directionFromVelocity(velocity);
}

int Boid::getId() const
{
    return m_id;
}

const glm::vec2 &Boid::getPosition() const
{
    return m_position;
}

const glm::vec2 &Boid::getVelocity() const
{
    return m_velocity;
}

glm::vec2 Boid::getDirection() const
{
    return m_direction;
}

float Boid::getSpeed() const
{
    return glm::length(m_velocity);
}

void Boid::setPosition(const glm::vec2 &newPosition)
{
    m_position = newPosition;
}

void Boid::setVelocity(const glm::vec2 &newVelocity)
{
    m_velocity = newVelocity;
    m_direction = directionFromVelocity(newVelocity);
}

void Boid::translate(const glm::vec2 &delta)
{
    m_position += delta;
}
