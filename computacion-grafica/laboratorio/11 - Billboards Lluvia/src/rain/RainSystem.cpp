#include "RainSystem.hpp"

#include <random>

#include "app/AppConfig.hpp"

RainSystem::RainSystem()
    : m_spawnRadius(appconfig::rain::SPAWN_RADIUS),
      m_spawnTop(appconfig::rain::SPAWN_TOP),
      m_groundHeight(appconfig::rain::GROUND_HEIGHT),
      m_rng(std::random_device{}())
{
}

bool RainSystem::initialize(std::size_t dropCount)
{
    if (dropCount == 0U)
    {
        return false;
    }

    m_drops.resize(dropCount);

    for (std::size_t index = 0; index < m_drops.size(); ++index)
    {
        respawnDrop(m_drops[index], true);
    }

    return true;
}

void RainSystem::update(float deltaTime)
{
    for (std::size_t index = 0; index < m_drops.size(); ++index)
    {
        Raindrop &drop = m_drops[index];
        drop.position.y -= drop.speed * deltaTime;

        if (drop.position.y + drop.halfHeight < m_groundHeight)
        {
            const float overshootDistance = m_groundHeight - (drop.position.y + drop.halfHeight);
            const float overshootTime = overshootDistance / drop.speed;
            respawnDrop(drop, false, overshootTime);
        }
    }
}

const std::vector<RainSystem::Raindrop> &RainSystem::getDrops() const
{
    return m_drops;
}

float RainSystem::randomRange(float minimum, float maximum)
{
    std::uniform_real_distribution<float> distribution(minimum, maximum);
    return distribution(m_rng);
}

void RainSystem::respawnDrop(Raindrop &drop, bool randomizeHeight, float overshootTime)
{
    drop.position.x = randomRange(-m_spawnRadius, m_spawnRadius);
    drop.position.z = randomRange(-m_spawnRadius, m_spawnRadius);
    drop.speed = randomRange(appconfig::rain::MIN_SPEED, appconfig::rain::MAX_SPEED);

    if (randomizeHeight)
    {
        drop.position.y = randomRange(appconfig::rain::SPAWN_MIN_HEIGHT, m_spawnTop);
    }
    else
    {
        drop.position.y = m_spawnTop - drop.speed * overshootTime;
    }

    drop.halfWidth = randomRange(appconfig::rain::MIN_HALF_WIDTH, appconfig::rain::MAX_HALF_WIDTH);
    drop.halfHeight = randomRange(appconfig::rain::MIN_HALF_HEIGHT, appconfig::rain::MAX_HALF_HEIGHT);
}
