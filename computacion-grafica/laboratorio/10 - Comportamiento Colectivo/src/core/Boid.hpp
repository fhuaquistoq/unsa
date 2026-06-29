#pragma once

#include <glm/vec2.hpp>

class Boid
{
public:
    Boid(int id, const glm::vec2 &position, const glm::vec2 &velocity);

    int getId() const;
    const glm::vec2 &getPosition() const;
    const glm::vec2 &getVelocity() const;
    glm::vec2 getDirection() const;
    float getSpeed() const;

    void setPosition(const glm::vec2 &newPosition);
    void setVelocity(const glm::vec2 &newVelocity);
    void translate(const glm::vec2 &delta);

private:
    int m_id;
    glm::vec2 m_position;
    glm::vec2 m_velocity;
    glm::vec2 m_direction;
};
