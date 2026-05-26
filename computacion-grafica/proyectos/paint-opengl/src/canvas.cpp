#include "canvas.h"
#include <glad/glad.h>
#include <cstdio>

Canvas::Canvas() = default;

Canvas::~Canvas()
{
    cleanup_fbo();
    if (quad_vao_)
        glDeleteVertexArrays(1, &quad_vao_);
    if (quad_vbo_)
        glDeleteBuffers(1, &quad_vbo_);
    if (canvas_shader_)
        glDeleteProgram(canvas_shader_);
}

void Canvas::init(int width, int height)
{
    width_ = width;
    height_ = height;
    setup_fbo();
    setup_quad();
    canvas_shader_ = compile_canvas_shader();
    renderer_.init();
    renderer_.set_viewport(width, height);
}

void Canvas::resize(int width, int height)
{
    if (width_ == width && height_ == height)
        return;
    width_ = width;
    height_ = height;
    cleanup_fbo();
    setup_fbo();
    renderer_.set_viewport(width, height);
}

void Canvas::begin_draw() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glViewport(0, 0, width_, height_);
}

void Canvas::end_draw() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::render(const Shape* preview)
{
    // 1. Dibujar al FBO: limpiar, luego dibujar todas las figuras completadas
    begin_draw();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Habilitar blending para transparencia
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (const auto &shape : shapes_)
    {
        if (shape->is_complete())
        {
            shape->render(&renderer_);
        }
    }
    if (preview)
    {
        preview->render(&renderer_);
    }
    end_draw();

    // Dibujar la textura del canvas en pantalla
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width_, height_);
    glUseProgram(canvas_shader_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);

    glBindVertexArray(quad_vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Canvas::add_shape(std::unique_ptr<Shape> shape)
{
    shapes_.push_back(std::move(shape));
}

void Canvas::clear_shapes()
{
    shapes_.clear();
}

void Canvas::clear()
{
    shapes_.clear();
    // Clear FBO to white
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::setup_fbo()
{
    // Textura
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Renderbuffer (depth/stencil)
    glGenRenderbuffers(1, &rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);

    // Framebuffer
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture_, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::fprintf(stderr, "Error: Framebuffer incompleto\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Limpiar canvas con fondo blanco
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Canvas::setup_quad()
{
    // Quad que cubre toda la pantalla (NDC)
    float vertices[] = {
        // pos          // tex_coord
        -1.0f,
        1.0f,
        0.0f,
        1.0f, // top-left
        -1.0f,
        -1.0f,
        0.0f,
        0.0f, // bottom-left
        1.0f,
        1.0f,
        1.0f,
        1.0f, // top-right
        1.0f,
        -1.0f,
        1.0f,
        0.0f, // bottom-right
    };

    glGenVertexArrays(1, &quad_vao_);
    glGenBuffers(1, &quad_vbo_);
    glBindVertexArray(quad_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    // tex_coord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Canvas::cleanup_fbo()
{
    if (fbo_)
    {
        glDeleteFramebuffers(1, &fbo_);
        fbo_ = 0;
    }
    if (texture_)
    {
        glDeleteTextures(1, &texture_);
        texture_ = 0;
    }
    if (rbo_)
    {
        glDeleteRenderbuffers(1, &rbo_);
        rbo_ = 0;
    }
}

GLuint Canvas::compile_canvas_shader() const
{
    // Shaders inline mínimos para mostrar la textura del canvas.
    // En producción, estos deberían cargarse desde assets/shaders/
    const char *vert_src = R"(
        #version 330 core
        layout (location = 0) in vec2 a_position;
        layout (location = 1) in vec2 a_tex_coord;
        out vec2 v_tex_coord;
        void main() {
            gl_Position = vec4(a_position, 0.0, 1.0);
            v_tex_coord = a_tex_coord;
        }
    )";

    const char *frag_src = R"(
        #version 330 core
        uniform sampler2D u_texture;
        in vec2 v_tex_coord;
        out vec4 frag_color;
        void main() {
            frag_color = texture(u_texture, v_tex_coord);
        }
    )";

    auto compile = [](GLenum type, const char *src) -> GLuint
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(shader, 512, nullptr, log);
            std::fprintf(stderr, "Shader compile error: %s\n", log);
        }
        return shader;
    };

    GLuint vs = compile(GL_VERTEX_SHADER, vert_src);
    GLuint fs = compile(GL_FRAGMENT_SHADER, frag_src);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::fprintf(stderr, "Shader link error: %s\n", log);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}