#pragma once

#include <glad/gl.h>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;

    bool load(const char *vertexSource, const char *fragmentSource);
    void use() const;
    GLuint getId() const;

private:
    GLuint compile(GLenum type, const char *source) const;
    void destroy();

    GLuint m_program;
};
