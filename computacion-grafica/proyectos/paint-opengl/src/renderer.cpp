#include "renderer.h"
#include <glad/glad.h>
#include <cstdio>

Renderer::Renderer() = default;

Renderer::~Renderer()
{
    if (vao_)
        glDeleteVertexArrays(1, &vao_);
    if (vbo_)
        glDeleteBuffers(1, &vbo_);
}

void Renderer::init()
{
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);

    // Cargar shaders de formas
    if (!shape_shader_.load("assets/shaders/shape.vert", "assets/shaders/shape.frag"))
    {
        std::fprintf(stderr, "Error: Failed to load shape shader\n");
    }
}

void Renderer::set_viewport(int width, int height)
{
    view_width_ = width;
    view_height_ = height;
    compute_orthographic_projection();
}

void Renderer::compute_orthographic_projection()
{
    // Orthographic projection matrix (column-major)
    // ortho(0, width, 0, height) with near=-1, far=1
    float left = 0.0f;
    float right = static_cast<float>(view_width_);
    float bottom = 0.0f;
    float top = static_cast<float>(view_height_);
    float near = -1.0f;
    float far = 1.0f;

    // col0
    projection_[0] = 2.0f / (right - left);
    projection_[1] = 0.0f;
    projection_[2] = 0.0f;
    projection_[3] = 0.0f;

    // col1
    projection_[4] = 0.0f;
    projection_[5] = 2.0f / (top - bottom);
    projection_[6] = 0.0f;
    projection_[7] = 0.0f;

    // col2
    projection_[8] = 0.0f;
    projection_[9] = 0.0f;
    projection_[10] = -2.0f / (far - near);
    projection_[11] = 0.0f;

    // col3
    projection_[12] = -(right + left) / (right - left);
    projection_[13] = -(top + bottom) / (top - bottom);
    projection_[14] = -(far + near) / (far - near);
    projection_[15] = 1.0f;
}

void Renderer::draw_lines(const std::vector<float> &vertices,
                          const Color &color) const
{
    if (vertices.empty())
        return;

    shape_shader_.use();
    glUniform4f(glGetUniformLocation(shape_shader_.program_id(), "u_color"),
                color.r, color.g, color.b, color.a);
    glUniformMatrix4fv(glGetUniformLocation(shape_shader_.program_id(), "u_projection"),
                        1, GL_FALSE, projection_);

    upload_vertices(vertices);

    glBindVertexArray(vao_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertices.size() / 2));
    glBindVertexArray(0);
}

void Renderer::draw_triangles(const std::vector<float> &vertices,
                              const Color &color) const
{
    if (vertices.empty())
        return;

    shape_shader_.use();
    glUniform4f(glGetUniformLocation(shape_shader_.program_id(), "u_color"),
                color.r, color.g, color.b, color.a);
    glUniformMatrix4fv(glGetUniformLocation(shape_shader_.program_id(), "u_projection"),
                        1, GL_FALSE, projection_);

    upload_vertices(vertices);

    glBindVertexArray(vao_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 2));
    glBindVertexArray(0);
}

void Renderer::draw_line_strip(const std::vector<float> &vertices,
                               const Color &color) const
{
    if (vertices.empty())
        return;

    shape_shader_.use();
    glUniform4f(glGetUniformLocation(shape_shader_.program_id(), "u_color"),
                color.r, color.g, color.b, color.a);
    glUniformMatrix4fv(glGetUniformLocation(shape_shader_.program_id(), "u_projection"),
                        1, GL_FALSE, projection_);

    upload_vertices(vertices);

    glBindVertexArray(vao_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(vertices.size() / 2));
    glBindVertexArray(0);
}

void Renderer::set_projection(const float *matrix) const
{
    // Projection is handled in draw methods now
}

void Renderer::upload_vertices(const std::vector<float> &vertices) const
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                 vertices.data(), GL_DYNAMIC_DRAW);
}