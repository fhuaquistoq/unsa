#pragma once
#include <glad/glad.h>
#include <string>

// Compila vertex + fragment shaders yadministra el program object.
class Shader
{
public:
    Shader();
    ~Shader();

    bool load(const std::string &vertex_path, const std::string &fragment_path);
    void use() const;

    void set_uniform(const std::string &name, float v0, float v1, float v2, float v3) const;
    void set_uniform(const std::string &name, int value) const;

    GLuint program_id() const { return program_id_; }

private:
    GLuint program_id_ = 0;

    GLuint compile_shader(GLenum type, const std::string &source) const;
    bool link_program(GLuint vertex_shader, GLuint fragment_shader);
    std::string read_file(const std::string &path) const;
};