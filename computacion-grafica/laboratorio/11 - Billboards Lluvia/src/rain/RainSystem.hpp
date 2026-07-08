#pragma once

#include <cstddef>
#include <random>
#include <vector>

#include <glm/vec3.hpp>

class RainSystem
{
public:
    struct Raindrop
    {
        glm::vec3 position;
        float speed;
        float halfWidth;
        float halfHeight;
    };

    RainSystem();

    bool initialize(std::size_t dropCount);
    void update(float deltaTime);
    const std::vector<Raindrop> &getDrops() const;

private:
    float randomRange(float minimum, float maximum);
    void respawnDrop(Raindrop &drop, bool randomizeHeight, float overshootTime = 0.0f);

    std::vector<Raindrop> m_drops;
    float m_spawnRadius;
    float m_spawnTop;
    float m_groundHeight;
    std::mt19937 m_rng;
};
