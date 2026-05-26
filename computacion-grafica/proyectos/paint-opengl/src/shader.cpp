#include "shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <cstdio>

Shader::Shader() = default;

Shader::~Shader()
{
    if (program_id_)
    {
        glDeleteProgram(program_id_);
    }
}

bool Shader::load(const std::string &vertex_path, const std::string &fragment_path)
{
    std::string vertex_source = read_file(vertex_path);
    std::string fragment_source = read_file(fragment_path);

    if (vertex_source.empty() || fragment_source.empty())
    {
        std::fprintf(stderr, "Error: no se pudo leer shaders\n");
        return false;
    }

    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_source);

    if (!vertex_shader || !fragment_shader)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return false;
    }

    if (!link_program(vertex_shader, fragment_shader))
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return false;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return true;
}

void Shader::use() const
{
    glUseProgram(program_id_);
}

void Shader::set_uniform(const std::string &name, float v0, float v1, float v2, float v3) const
{
    glUniform4f(glGetUniformLocation(program_id_, name.c_str()), v0, v1, v2, v3);
}

void Shader::set_uniform(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(program_id_, name.c_str()), value);
}

GLuint Shader::compile_shader(GLenum type, const std::string &source) const
{
    GLuint shader = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::fprintf(stderr, "Error compilando shader: %s\n", log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::link_program(GLuint vertex_shader, GLuint fragment_shader)
{
    program_id_ = glCreateProgram();
    glAttachShader(program_id_, vertex_shader);
    glAttachShader(program_id_, fragment_shader);
    glLinkProgram(program_id_);

    GLint success;
    glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(program_id_, 512, nullptr, log);
        std::fprintf(stderr, "Error linkando programa: %s\n", log);
        glDeleteProgram(program_id_);
        program_id_ = 0;
        return false;
    }
    return true;
}

std::string Shader::read_file(const std::string &path) const
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::fprintf(stderr, "Error: no se pudo abrir %s\n", path.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}