#pragma once
#include <glad/glad.h>
#include "shape.h"
#include "shader.h"
#include <vector>

// Encapsula VAO + VBO para renderizar primitivas 2D.
// Encargado de subir vértices a la GPU y hacer los draw calls.
class Renderer
{
public:
    Renderer();
    ~Renderer();

    void init();

    // Configura el viewport y recalcula la proyección ortográfica
    void set_viewport(int width, int height);

    // Dibuja un conjunto de líneas (GL_LINES)
    void draw_lines(const std::vector<float> &vertices, const Color &color) const;

    // Dibuja un conjunto de triángulos (GL_TRIANGLES)
    void draw_triangles(const std::vector<float> &vertices, const Color &color) const;

    // Dibuja una strip de líneas (GL_LINE_STRIP) para Brush
    void draw_line_strip(const std::vector<float> &vertices, const Color &color) const;

    void set_projection(const float *matrix) const;

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    Shader shape_shader_;

    int view_width_ = 0;
    int view_height_ = 0;
    float projection_[16];

    void compute_orthographic_projection();
    void upload_vertices(const std::vector<float> &vertices) const;
};