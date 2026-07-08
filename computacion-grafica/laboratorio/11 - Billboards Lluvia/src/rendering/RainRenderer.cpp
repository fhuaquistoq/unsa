#include "RainRenderer.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>

#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "app/AppConfig.hpp"
#include "scene/OrbitCamera.hpp"

namespace
{
    constexpr const char *VERTEX_SHADER = R"(
        #version 330 core
        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec2 aTexCoord;

        uniform mat4 uView;
        uniform mat4 uProjection;

        out vec2 vTexCoord;

        void main()
        {
            vTexCoord = aTexCoord;
            gl_Position = uProjection * uView * vec4(aPosition, 1.0);
        }
    )";

    constexpr const char *FRAGMENT_SHADER = R"(
        #version 330 core
        in vec2 vTexCoord;

        uniform sampler2D uRaindropTexture;
        uniform float uAlphaCutoff;
        uniform float uAlphaScale;
        uniform vec3 uTint;

        out vec4 fragmentColor;

        void main()
        {
            vec4 sampleColor = texture(uRaindropTexture, vTexCoord);

            if (sampleColor.a < uAlphaCutoff)
            {
                discard;
            }

            fragmentColor = vec4(uTint * sampleColor.rgb, sampleColor.a * uAlphaScale);
        }
    )";
}

RainRenderer::RainRenderer()
    : m_vao(0),
      m_vbo(0)
{
}

RainRenderer::~RainRenderer()
{
    destroy();
}

bool RainRenderer::initialize()
{
    if (!m_shader.load(VERTEX_SHADER, FRAGMENT_SHADER) || !createRaindropTexture())
    {
        return false;
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          static_cast<GLsizei>(sizeof(RainVertex)), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          static_cast<GLsizei>(sizeof(RainVertex)), reinterpret_cast<void *>(offsetof(RainVertex, uv)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    return true;
}

void RainRenderer::render(const RainSystem &rainSystem,
                          const OrbitCamera &camera,
                          const glm::mat4 &view,
                          const glm::mat4 &projection)
{
    buildBillboardVertices(rainSystem, camera);

    m_shader.use();
    m_texture.bind(0);

    const GLint viewLocation = glGetUniformLocation(m_shader.getId(), "uView");
    const GLint projectionLocation = glGetUniformLocation(m_shader.getId(), "uProjection");
    const GLint textureLocation = glGetUniformLocation(m_shader.getId(), "uRaindropTexture");
    const GLint alphaCutoffLocation = glGetUniformLocation(m_shader.getId(), "uAlphaCutoff");
    const GLint alphaScaleLocation = glGetUniformLocation(m_shader.getId(), "uAlphaScale");
    const GLint tintLocation = glGetUniformLocation(m_shader.getId(), "uTint");

    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(textureLocation, 0);
    glUniform1f(alphaCutoffLocation, appconfig::rain::MIN_ALPHA);
    glUniform1f(alphaScaleLocation, appconfig::rain::ALPHA_SCALE);
    glUniform3f(tintLocation, appconfig::rain::TINT.r, appconfig::rain::TINT.g, appconfig::rain::TINT.b);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(m_vertices.size() * sizeof(RainVertex)),
                 m_vertices.data(),
                 GL_DYNAMIC_DRAW);

    glDepthMask(GL_FALSE);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertices.size()));
    glDepthMask(GL_TRUE);

    glBindVertexArray(0);
}

void RainRenderer::buildBillboardVertices(const RainSystem &rainSystem, const OrbitCamera &camera)
{
    struct SortedDrop
    {
        const RainSystem::Raindrop *drop;
        float distanceSquared;
    };

    std::vector<SortedDrop> sortedDrops;
    const std::vector<RainSystem::Raindrop> &drops = rainSystem.getDrops();
    sortedDrops.reserve(drops.size());

    const glm::vec3 cameraPosition = camera.getPosition();

    for (const RainSystem::Raindrop &drop : drops)
    {
        const glm::vec3 delta = drop.position - cameraPosition;
        sortedDrops.push_back(SortedDrop{&drop, glm::dot(delta, delta)});
    }

    std::sort(sortedDrops.begin(), sortedDrops.end(), [](const SortedDrop &left, const SortedDrop &right)
              { return left.distanceSquared > right.distanceSquared; });

    m_vertices.clear();
    m_vertices.reserve(sortedDrops.size() * 6U);

    for (const SortedDrop &entry : sortedDrops)
    {
        const RainSystem::Raindrop &drop = *entry.drop;
        glm::vec3 toCamera = cameraPosition - drop.position;
        toCamera.y = 0.0f;

        if (glm::dot(toCamera, toCamera) < appconfig::rain::BILLBOARD_FORWARD_FALLBACK_DISTANCE_SQUARED)
        {
            toCamera = -camera.getForward();
            toCamera.y = 0.0f;
        }

        const glm::vec3 forward = glm::normalize(toCamera);
        const glm::vec3 horizontal = glm::normalize(glm::cross(appconfig::WORLD_UP, forward)) * drop.halfWidth;
        const glm::vec3 vertical = appconfig::WORLD_UP * drop.halfHeight;

        const glm::vec3 topLeft = drop.position - horizontal + vertical;
        const glm::vec3 topRight = drop.position + horizontal + vertical;
        const glm::vec3 bottomLeft = drop.position - horizontal - vertical;
        const glm::vec3 bottomRight = drop.position + horizontal - vertical;

        // Estas esquinas se reconstruyen en CPU para mantener la lluvia vertical
        // en el mundo mientras cada billboard sigue orientándose hacia la cámara.
        m_vertices.push_back(RainVertex{topLeft, glm::vec2(0.0f, 1.0f)});
        m_vertices.push_back(RainVertex{bottomLeft, glm::vec2(0.0f, 0.0f)});
        m_vertices.push_back(RainVertex{topRight, glm::vec2(1.0f, 1.0f)});
        m_vertices.push_back(RainVertex{topRight, glm::vec2(1.0f, 1.0f)});
        m_vertices.push_back(RainVertex{bottomLeft, glm::vec2(0.0f, 0.0f)});
        m_vertices.push_back(RainVertex{bottomRight, glm::vec2(1.0f, 0.0f)});
    }
}

bool RainRenderer::createRaindropTexture()
{
    constexpr int textureWidth = appconfig::rain::TEXTURE_WIDTH;
    constexpr int textureHeight = appconfig::rain::TEXTURE_HEIGHT;
    std::array<unsigned char, textureWidth * textureHeight * 4> pixels = {};

    // La textura se genera en código para evitar assets externos y mostrar cómo
    // una gota puede salir de una simple máscara alfa procedural.
    for (int y = 0; y < textureHeight; ++y)
    {
        for (int x = 0; x < textureWidth; ++x)
        {
            const float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(textureWidth);
            const float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(textureHeight);

            const float centeredX = (u - 0.5f) * 2.0f;
            const float dropWidth = appconfig::rain::TEXTURE_BASE_WIDTH +
                                    appconfig::rain::TEXTURE_WIDTH_GAIN * (1.0f - v);
            const float horizontalMask = std::max(0.0f, 1.0f - std::fabs(centeredX) / dropWidth);
            const float verticalFade = std::pow(v, appconfig::rain::TEXTURE_VERTICAL_FADE_POWER);
            const float alpha = std::pow(horizontalMask * verticalFade, appconfig::rain::TEXTURE_ALPHA_POWER);

            const std::size_t pixelIndex = static_cast<std::size_t>((y * textureWidth + x) * 4);
            pixels[pixelIndex + 0] = appconfig::rain::TEXTURE_RED;
            pixels[pixelIndex + 1] = appconfig::rain::TEXTURE_GREEN;
            pixels[pixelIndex + 2] = appconfig::rain::TEXTURE_BLUE;
            pixels[pixelIndex + 3] = static_cast<unsigned char>(std::round(alpha * 255.0f));
        }
    }

    return m_texture.createFromRGBA(pixels.data(), textureWidth, textureHeight);
}

void RainRenderer::destroy()
{
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
