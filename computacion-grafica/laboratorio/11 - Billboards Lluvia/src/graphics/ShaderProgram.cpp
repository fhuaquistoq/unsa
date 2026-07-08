#include <algorithm>
#include "ShaderProgram.hpp"

#include <iostream>
#include <vector>

namespace
{
    void printShaderLog(GLuint object, bool isProgram)
    {
        GLint logLength = 0;

        if (isProgram)
        {
            glGetProgramiv(object, GL_INFO_LOG_LENGTH, &logLength);
        }
        else
        {
            glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);
        }

        std::vector<char> log(static_cast<std::size_t>(std::max(logLength, 1)), '\0');

        if (isProgram)
        {
            glGetProgramInfoLog(object, logLength, nullptr, log.data());
            std::cout << "Program link failed: " << log.data() << "\n";
        }
        else
        {
            glGetShaderInfoLog(object, logLength, nullptr, log.data());
            std::cout << "Shader compile failed: " << log.data() << "\n";
        }
    }
}

ShaderProgram::ShaderProgram()
    : m_program(0)
{
}

ShaderProgram::~ShaderProgram()
{
    destroy();
}

bool ShaderProgram::load(const char *vertexSource, const char *fragmentSource)
{
    destroy();

    const GLuint vertexShader = compile(GL_VERTEX_SHADER, vertexSource);
    const GLuint fragmentShader = compile(GL_FRAGMENT_SHADER, fragmentSource);

    if (vertexShader == 0 || fragmentShader == 0)
    {
        if (vertexShader != 0)
        {
            glDeleteShader(vertexShader);
        }

        if (fragmentShader != 0)
        {
            glDeleteShader(fragmentShader);
        }

        return false;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linked = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

    if (linked != GL_TRUE)
    {
        printShaderLog(m_program, true);
        destroy();
        return false;
    }

    return true;
}

void ShaderProgram::use() const
{
    glUseProgram(m_program);
}

GLuint ShaderProgram::getId() const
{
    return m_program;
}

GLuint ShaderProgram::compile(GLenum type, const char *source) const
{
    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (compiled != GL_TRUE)
    {
        printShaderLog(shader, false);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void ShaderProgram::destroy()
{
    if (m_program != 0)
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}
