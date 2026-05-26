#pragma once
#include <glad/glad.h>
#include <vector>
#include <memory>
#include "shape.h"
#include "renderer.h"

// Canvas: la superficie donde se dibuja todo.
// Usa un FBO + textura para renderizar offscreen y luego mostrarla.
class Canvas
{
public:
    Canvas();
    ~Canvas();

    void init(int width, int height);
    void resize(int width, int height);
    void render(const Shape* preview = nullptr);

    void add_shape(std::unique_ptr<Shape> shape);
    void clear_shapes();
    void clear();
    void begin_draw() const; // Bind FBO para dibujar offscreen
    void end_draw() const;   // Unbind FBO

private:
    int width_ = 0;
    int height_ = 0;
    GLuint fbo_ = 0;
    GLuint texture_ = 0;
    GLuint rbo_ = 0; // Renderbuffer para depth/stencil
    GLuint quad_vao_ = 0;
    GLuint quad_vbo_ = 0;
    GLuint canvas_shader_ = 0;

    Renderer renderer_;

    std::vector<std::unique_ptr<Shape>> shapes_;

    void setup_fbo();
    void setup_quad();
    void cleanup_fbo();
    GLuint compile_canvas_shader() const;
};