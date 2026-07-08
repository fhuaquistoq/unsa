#pragma once

#include <vector>

#include <glad/gl.h>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "graphics/ShaderProgram.hpp"
#include "graphics/Texture2D.hpp"
#include "rain/RainSystem.hpp"

class OrbitCamera;

struct RainVertex
{
    glm::vec3 position;
    glm::vec2 uv;
};

class RainRenderer
{
public:
    RainRenderer();
    ~RainRenderer();

    RainRenderer(const RainRenderer &) = delete;
    RainRenderer &operator=(const RainRenderer &) = delete;

    bool initialize();
    void render(const RainSystem &rainSystem,
                const OrbitCamera &camera,
                const glm::mat4 &view,
                const glm::mat4 &projection);

private:
    void buildBillboardVertices(const RainSystem &rainSystem, const OrbitCamera &camera);
    bool createRaindropTexture();
    void destroy();

    ShaderProgram m_shader;
    Texture2D m_texture;
    GLuint m_vao;
    GLuint m_vbo;
    std::vector<RainVertex> m_vertices;
};
