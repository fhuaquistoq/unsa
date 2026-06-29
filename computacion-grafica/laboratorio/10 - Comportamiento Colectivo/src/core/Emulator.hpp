#pragma once

#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include "Boid.hpp"

class Emulator
{
public:
    struct Parameters
    {
        int boidCount = 80;
        float neighborhoodDistance = 90.0f;
        float separationWeight = 1.6f;
        float alignmentWeight = 0.9f;
        float cohesionWeight = 0.7f;
        float separationDistance = 28.0f;
        float minSpeed = 80.0f;
        float maxSpeed = 200.0f;
    };

    Emulator(float worldWidth, float worldHeight);

    void initialize();
    void update(float deltaTimeSeconds);
    std::vector<float> buildRenderVertices() const;

    const std::vector<Boid> &getBoids() const;
    const Parameters &getParameters() const;

    bool setBoidCount(int newCount);
    bool setNeighborhoodDistance(float newDistance);
    bool setSeparationWeight(float newWeight);
    bool setAlignmentWeight(float newWeight);
    bool setCohesionWeight(float newWeight);

    std::string getParameterSummary() const;

private:
    struct Influence
    {
        glm::vec2 separation{0.0f, 0.0f};
        glm::vec2 alignment{0.0f, 0.0f};
        glm::vec2 cohesion{0.0f, 0.0f};
    };

    float m_worldWidth;
    float m_worldHeight;
    Parameters m_parameters;
    std::vector<Boid> m_boids;

    Boid createRandomBoid(int id) const;
    void addBoids(int count);
    void removeBoids(int count);
    Influence computeInfluence(std::size_t boidIndex) const;
    glm::vec2 shortestWrappedOffset(const glm::vec2 &from, const glm::vec2 &to) const;
    glm::vec2 clampVelocity(const glm::vec2 &velocity, const glm::vec2 &fallbackDirection) const;
    glm::vec2 wrapPosition(const glm::vec2 &position) const;
    glm::vec2 randomPosition() const;
    glm::vec2 randomVelocity() const;
};
