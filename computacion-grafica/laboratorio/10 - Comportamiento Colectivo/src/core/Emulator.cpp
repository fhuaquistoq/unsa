#include "Emulator.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <random>
#include <sstream>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

namespace
{
    constexpr float PI = 3.14159265358979323846f;
    constexpr float BOID_LENGTH = 14.0f;
    constexpr float BOID_WIDTH = 8.0f;

    std::mt19937 &randomEngine()
    {
        static std::mt19937 engine{std::random_device{}()};
        return engine;
    }

    float randomFloat(float minimum, float maximum)
    {
        std::uniform_real_distribution<float> distribution(minimum, maximum);
        return distribution(randomEngine());
    }

    glm::vec2 safeNormalize(const glm::vec2 &value, const glm::vec2 &fallback)
    {
        const float length = glm::length(value);
        if (length <= 0.0001f)
            return fallback;

        return value / length;
    }
}

Emulator::Emulator(float worldWidth, float worldHeight)
{
    this->m_worldWidth = std::max(1.0f, worldWidth);
    this->m_worldHeight = std::max(1.0f, worldHeight);
}

void Emulator::initialize()
{
    m_boids.clear();
    m_boids.reserve(static_cast<std::size_t>(m_parameters.boidCount));
    addBoids(m_parameters.boidCount);
}

void Emulator::update(float deltaTimeSeconds)
{
    if (deltaTimeSeconds <= 0.0f || m_boids.empty())
        return;

    std::vector<glm::vec2> nextVelocities(m_boids.size());
    std::vector<glm::vec2> nextPositions(m_boids.size());

    for (std::size_t i = 0; i < m_boids.size(); ++i)
    {
        const Boid &boid = m_boids[i];
        const Influence influence = computeInfluence(i);
        const glm::vec2 baseDirection = boid.getDirection();

        glm::vec2 updatedVelocity = boid.getVelocity();
        updatedVelocity += influence.separation * m_parameters.separationWeight;
        updatedVelocity += influence.alignment * m_parameters.alignmentWeight;
        updatedVelocity += influence.cohesion * m_parameters.cohesionWeight;

        updatedVelocity = clampVelocity(updatedVelocity, baseDirection);
        nextVelocities[i] = updatedVelocity;
        nextPositions[i] = wrapPosition(boid.getPosition() + updatedVelocity * deltaTimeSeconds);
    }

    for (std::size_t i = 0; i < m_boids.size(); ++i)
    {
        m_boids[i].setVelocity(nextVelocities[i]);
        m_boids[i].setPosition(nextPositions[i]);
    }
}

std::vector<float> Emulator::buildRenderVertices() const
{
    std::vector<float> vertices;
    vertices.reserve(m_boids.size() * 18);

    for (const Boid &boid : m_boids)
    {
        const glm::vec2 direction = boid.getDirection();
        const glm::vec2 normal{-direction.y, direction.x};
        const glm::vec2 position = boid.getPosition();

        const glm::vec2 tip = position + direction * BOID_LENGTH;
        const glm::vec2 left = position - direction * (BOID_LENGTH * 0.45f) + normal * (BOID_WIDTH * 0.5f);
        const glm::vec2 right = position - direction * (BOID_LENGTH * 0.45f) - normal * (BOID_WIDTH * 0.5f);

        const glm::vec3 color{0.75f, 0.9f, 1.0f};

        auto appendVertex = [&vertices, &color](const glm::vec2 &point)
        {
            vertices.push_back(point.x);
            vertices.push_back(point.y);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
        };

        appendVertex(tip);
        appendVertex(left);
        appendVertex(right);
    }

    return vertices;
}

const std::vector<Boid> &Emulator::getBoids() const
{
    return m_boids;
}

const Emulator::Parameters &Emulator::getParameters() const
{
    return m_parameters;
}

bool Emulator::setBoidCount(int newCount)
{
    if (newCount < 1)
        return false;

    const int currentCount = static_cast<int>(m_boids.size());
    m_parameters.boidCount = newCount;

    if (newCount > currentCount)
        addBoids(newCount - currentCount);
    else if (newCount < currentCount)
        removeBoids(currentCount - newCount);

    return true;
}

bool Emulator::setNeighborhoodDistance(float newDistance)
{
    if (newDistance <= 1.0f)
        return false;

    m_parameters.neighborhoodDistance = newDistance;
    m_parameters.separationDistance = std::min(m_parameters.separationDistance, newDistance);
    return true;
}

bool Emulator::setSeparationWeight(float newWeight)
{
    if (newWeight < 0.0f)
        return false;

    m_parameters.separationWeight = newWeight;
    return true;
}

bool Emulator::setAlignmentWeight(float newWeight)
{
    if (newWeight < 0.0f)
        return false;

    m_parameters.alignmentWeight = newWeight;
    return true;
}

bool Emulator::setCohesionWeight(float newWeight)
{
    if (newWeight < 0.0f)
        return false;

    m_parameters.cohesionWeight = newWeight;
    return true;
}

std::string Emulator::getParameterSummary() const
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2)
           << "boids=" << m_parameters.boidCount
           << ", neighborhood=" << m_parameters.neighborhoodDistance
           << ", separation=" << m_parameters.separationWeight
           << ", alignment=" << m_parameters.alignmentWeight
           << ", cohesion=" << m_parameters.cohesionWeight;
    return stream.str();
}

Boid Emulator::createRandomBoid(int id) const
{
    return Boid(id, randomPosition(), randomVelocity());
}

void Emulator::addBoids(int count)
{
    const int startId = static_cast<int>(m_boids.size());
    m_boids.reserve(m_boids.size() + static_cast<std::size_t>(count));

    for (int offset = 0; offset < count; ++offset)
        m_boids.push_back(createRandomBoid(startId + offset));
}

void Emulator::removeBoids(int count)
{
    if (count <= 0)
        return;

    const std::size_t keepCount = m_boids.size() - std::min<std::size_t>(m_boids.size(), static_cast<std::size_t>(count));
    m_boids.erase(m_boids.begin() + static_cast<std::ptrdiff_t>(keepCount), m_boids.end());
}

Emulator::Influence Emulator::computeInfluence(std::size_t boidIndex) const
{
    const Boid &self = m_boids[boidIndex];

    glm::vec2 separationSum{0.0f, 0.0f};
    glm::vec2 alignmentVelocitySum{0.0f, 0.0f};
    glm::vec2 cohesionOffsetSum{0.0f, 0.0f};
    int neighborCount = 0;
    int separationCount = 0;

    for (std::size_t otherIndex = 0; otherIndex < m_boids.size(); ++otherIndex)
    {
        if (otherIndex == boidIndex)
            continue;

        const glm::vec2 offset = shortestWrappedOffset(self.getPosition(), m_boids[otherIndex].getPosition());
        const float distance = glm::length(offset);

        if (distance <= 0.0001f || distance > m_parameters.neighborhoodDistance)
            continue;

        ++neighborCount;
        alignmentVelocitySum += m_boids[otherIndex].getVelocity();
        cohesionOffsetSum += offset;

        if (distance < m_parameters.separationDistance)
        {
            separationSum -= offset / distance;
            ++separationCount;
        }
    }

    Influence influence;

    if (separationCount > 0)
        influence.separation = safeNormalize(separationSum / static_cast<float>(separationCount), glm::vec2{0.0f, 0.0f}) * 35.0f;

    if (neighborCount > 0)
    {
        const glm::vec2 averageVelocity = alignmentVelocitySum / static_cast<float>(neighborCount);
        const glm::vec2 desiredAlignment = safeNormalize(averageVelocity, self.getDirection()) * self.getSpeed();
        influence.alignment = (desiredAlignment - self.getVelocity()) * 0.08f;

        const glm::vec2 averageOffset = cohesionOffsetSum / static_cast<float>(neighborCount);
        influence.cohesion = safeNormalize(averageOffset, glm::vec2{0.0f, 0.0f}) * 18.0f;
    }

    return influence;
}

glm::vec2 Emulator::shortestWrappedOffset(const glm::vec2 &from, const glm::vec2 &to) const
{
    glm::vec2 offset = to - from;

    if (offset.x > m_worldWidth * 0.5f)
        offset.x -= m_worldWidth;
    else if (offset.x < -m_worldWidth * 0.5f)
        offset.x += m_worldWidth;

    if (offset.y > m_worldHeight * 0.5f)
        offset.y -= m_worldHeight;
    else if (offset.y < -m_worldHeight * 0.5f)
        offset.y += m_worldHeight;

    return offset;
}

glm::vec2 Emulator::clampVelocity(const glm::vec2 &velocity, const glm::vec2 &fallbackDirection) const
{
    const glm::vec2 direction = safeNormalize(velocity, fallbackDirection);
    const float speed = glm::length(velocity);
    const float clampedSpeed = std::clamp(speed, m_parameters.minSpeed, m_parameters.maxSpeed);
    return direction * clampedSpeed;
}

glm::vec2 Emulator::wrapPosition(const glm::vec2 &position) const
{
    glm::vec2 wrapped = position;

    if (wrapped.x < 0.0f)
        wrapped.x += m_worldWidth;
    else if (wrapped.x >= m_worldWidth)
        wrapped.x -= m_worldWidth;

    if (wrapped.y < 0.0f)
        wrapped.y += m_worldHeight;
    else if (wrapped.y >= m_worldHeight)
        wrapped.y -= m_worldHeight;

    return wrapped;
}

glm::vec2 Emulator::randomPosition() const
{
    return {randomFloat(0.0f, m_worldWidth), randomFloat(0.0f, m_worldHeight)};
}

glm::vec2 Emulator::randomVelocity() const
{
    const float angle = randomFloat(0.0f, PI * 2.0f);
    const float speed = randomFloat(m_parameters.minSpeed, m_parameters.maxSpeed);
    return glm::vec2{std::cos(angle), std::sin(angle)} * speed;
}
