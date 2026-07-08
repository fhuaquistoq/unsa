#include "GroundRenderer.hpp"

#include <array>

#include <glm/gtc/type_ptr.hpp>

#include "app/AppConfig.hpp"

namespace
{
    constexpr const char *VERTEX_SHADER = R"(
        #version 330 core
        layout (location = 0) in vec3 aPosition;

        uniform mat4 uView;
        uniform mat4 uProjection;

        out vec3 vWorldPosition;

        void main()
        {
            vWorldPosition = aPosition;
            gl_Position = uProjection * uView * vec4(aPosition, 1.0);
        }
    )";

    constexpr const char *FRAGMENT_SHADER = R"(
        #version 330 core
        in vec3 vWorldPosition;

        out vec4 fragmentColor;

        void main()
        {
            float checker = mod(floor(vWorldPosition.x) + floor(vWorldPosition.z), 2.0);
            vec3 darkColor = vec3(0.09, 0.12, 0.10);
            vec3 lightColor = vec3(0.12, 0.16, 0.13);
            vec3 color = mix(darkColor, lightColor, checker);
            fragmentColor = vec4(color, 1.0);
        }
    )";
}

GroundRenderer::GroundRenderer()
    : m_vao(0),
      m_vbo(0),
      m_ebo(0)
{
}

GroundRenderer::~GroundRenderer()
{
    destroy();
}

bool GroundRenderer::initialize()
{
    static const std::array<float, 12> vertices = {
        -appconfig::ground::HALF_EXTENT, 0.0f, -appconfig::ground::HALF_EXTENT,
         appconfig::ground::HALF_EXTENT, 0.0f, -appconfig::ground::HALF_EXTENT,
         appconfig::ground::HALF_EXTENT, 0.0f,  appconfig::ground::HALF_EXTENT,
        -appconfig::ground::HALF_EXTENT, 0.0f,  appconfig::ground::HALF_EXTENT};

    static const std::array<unsigned int, 6> indices = {0, 1, 2, 0, 2, 3};

    if (!m_shader.load(VERTEX_SHADER, FRAGMENT_SHADER))
    {
        return false;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * static_cast<GLsizei>(sizeof(float)), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return true;
}

void GroundRenderer::render(const glm::mat4 &view, const glm::mat4 &projection) const
{
    m_shader.use();

    const GLint viewLocation = glGetUniformLocation(m_shader.getId(), "uView");
    const GLint projectionLocation = glGetUniformLocation(m_shader.getId(), "uProjection");

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void GroundRenderer::destroy()
{
    if (m_ebo != 0)
    {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }

    if (m_vbo != 0)
    {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }

    if (m_vao != 0)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}
