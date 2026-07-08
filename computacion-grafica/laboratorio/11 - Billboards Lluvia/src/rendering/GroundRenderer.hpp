#pragma once

#include <glad/gl.h>
#include <glm/mat4x4.hpp>

#include "graphics/ShaderProgram.hpp"

class GroundRenderer
{
public:
    GroundRenderer();
    ~GroundRenderer();

    GroundRenderer(const GroundRenderer &) = delete;
    GroundRenderer &operator=(const GroundRenderer &) = delete;

    bool initialize();
    void render(const glm::mat4 &view, const glm::mat4 &projection) const;

private:
    void destroy();

    ShaderProgram m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
};
