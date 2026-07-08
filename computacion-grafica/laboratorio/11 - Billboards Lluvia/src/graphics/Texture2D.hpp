#pragma once

#include <glad/gl.h>

class Texture2D
{
public:
    Texture2D();
    ~Texture2D();

    Texture2D(const Texture2D &) = delete;
    Texture2D &operator=(const Texture2D &) = delete;

    bool createFromRGBA(const unsigned char *data, int width, int height);
    void bind(GLuint unit) const;

private:
    void destroy();

    GLuint m_textureId;
};
