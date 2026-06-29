#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "core/Emulator.hpp"
#include "utils/DeltaTime.hpp"
#include "utils/OpenGLInit.hpp"

namespace
{
    constexpr int WINDOW_WIDTH = 800;
    constexpr int WINDOW_HEIGHT = 800;

    constexpr const char *VERTEX_SHADER_SOURCE = R"(
        #version 330 core
        layout (location = 0) in vec2 aPosition;
        layout (location = 1) in vec3 aColor;

        out vec3 vColor;

        uniform vec2 uWindowSize;

        void main()
        {
            vec2 normalized = (aPosition / uWindowSize) * 2.0 - 1.0;
            normalized.y = -normalized.y;
            gl_Position = vec4(normalized, 0.0, 1.0);
            vColor = aColor;
        }
    )";

    constexpr const char *FRAGMENT_SHADER_SOURCE = R"(
        #version 330 core
        in vec3 vColor;

        out vec4 fragmentColor;

        void main()
        {
            fragmentColor = vec4(vColor, 1.0);
        }
    )";

    enum class EditTarget
    {
        None,
        BoidCount,
        NeighborhoodDistance,
        SeparationWeight,
        AlignmentWeight,
        CohesionWeight,
    };

    struct AppState
    {
        Emulator emulator{static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT)};
        GLuint shaderProgram = 0;
        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLint windowSizeUniform = -1;
        int framebufferWidth = WINDOW_WIDTH;
        int framebufferHeight = WINDOW_HEIGHT;
        EditTarget editTarget = EditTarget::None;
        std::string editBuffer;
    };

    void updateFramebufferSize(AppState &appState, int width, int height)
    {
        appState.framebufferWidth = std::max(width, 1);
        appState.framebufferHeight = std::max(height, 1);
        glViewport(0, 0, appState.framebufferWidth, appState.framebufferHeight);
    }

    GLuint compileShader(GLenum type, const char *source)
    {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint compiled = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (compiled == GL_TRUE)
        {
            return shader;
        }

        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(std::max(logLength, 1)), '\0');
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());
        std::cout << "Shader compilation failed: " << log << "\n";
        glDeleteShader(shader);
        return 0;
    }

    GLuint createShaderProgram()
    {
        const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
        const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);

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

            return 0;
        }

        const GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        GLint linked = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);

        if (linked == GL_TRUE)
        {
            return program;
        }

        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::string log(static_cast<std::size_t>(std::max(logLength, 1)), '\0');
        glGetProgramInfoLog(program, logLength, nullptr, log.data());
        std::cout << "Program linking failed: " << log << "\n";
        glDeleteProgram(program);
        return 0;
    }

    const char *editTargetName(EditTarget target)
    {
        switch (target)
        {
        case EditTarget::BoidCount:
            return "boid count";
        case EditTarget::NeighborhoodDistance:
            return "neighborhood distance";
        case EditTarget::SeparationWeight:
            return "separation intensity";
        case EditTarget::AlignmentWeight:
            return "alignment intensity";
        case EditTarget::CohesionWeight:
            return "cohesion intensity";
        case EditTarget::None:
        default:
            return "none";
        }
    }

    void logEditBuffer(const AppState &appState)
    {
        if (appState.editTarget == EditTarget::None)
        {
            return;
        }

        std::cout << "Editing " << editTargetName(appState.editTarget) << ": "
                  << (appState.editBuffer.empty() ? "<empty>" : appState.editBuffer) << "\n";
    }

    void beginEdit(AppState &appState, EditTarget target)
    {
        appState.editTarget = target;
        appState.editBuffer.clear();
        std::cout << "Editing " << editTargetName(target)
                  << ". Type the value and press Enter. Escape cancels.\n";
    }

    void cancelEdit(AppState &appState)
    {
        if (appState.editTarget == EditTarget::None)
        {
            return;
        }

        std::cout << "Canceled " << editTargetName(appState.editTarget) << " update.\n";
        appState.editTarget = EditTarget::None;
        appState.editBuffer.clear();
    }

    bool applyEdit(AppState &appState)
    {
        if (appState.editTarget == EditTarget::None || appState.editBuffer.empty())
        {
            return false;
        }

        bool success = false;

        try
        {
            switch (appState.editTarget)
            {
            case EditTarget::BoidCount:
                success = appState.emulator.setBoidCount(std::stoi(appState.editBuffer));
                break;
            case EditTarget::NeighborhoodDistance:
                success = appState.emulator.setNeighborhoodDistance(std::stof(appState.editBuffer));
                break;
            case EditTarget::SeparationWeight:
                success = appState.emulator.setSeparationWeight(std::stof(appState.editBuffer));
                break;
            case EditTarget::AlignmentWeight:
                success = appState.emulator.setAlignmentWeight(std::stof(appState.editBuffer));
                break;
            case EditTarget::CohesionWeight:
                success = appState.emulator.setCohesionWeight(std::stof(appState.editBuffer));
                break;
            case EditTarget::None:
                break;
            }
        }
        catch (const std::exception &)
        {
            success = false;
        }

        if (success)
        {
            std::cout << "Updated parameters: " << appState.emulator.getParameterSummary() << "\n";
        }
        else
        {
            std::cout << "Invalid value for " << editTargetName(appState.editTarget)
                      << ": " << appState.editBuffer << "\n";
        }

        appState.editTarget = EditTarget::None;
        appState.editBuffer.clear();
        return success;
    }

    void keyCallback(GLFWwindow *window, int key, int, int action, int)
    {
        if (action != GLFW_PRESS && action != GLFW_REPEAT)
        {
            return;
        }

        auto *appState = static_cast<AppState *>(glfwGetWindowUserPointer(window));

        if (appState == nullptr)
        {
            return;
        }

        if (key == GLFW_KEY_ESCAPE)
        {
            if (appState->editTarget != EditTarget::None)
            {
                cancelEdit(*appState);
            }
            else if (action == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            return;
        }

        if (appState->editTarget != EditTarget::None)
        {
            if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER)
            {
                applyEdit(*appState);
            }
            else if (key == GLFW_KEY_BACKSPACE && !appState->editBuffer.empty())
            {
                appState->editBuffer.pop_back();
                logEditBuffer(*appState);
            }

            return;
        }

        if (action != GLFW_PRESS)
        {
            return;
        }

        switch (key)
        {
        case GLFW_KEY_N:
            beginEdit(*appState, EditTarget::BoidCount);
            break;
        case GLFW_KEY_D:
            beginEdit(*appState, EditTarget::NeighborhoodDistance);
            break;
        case GLFW_KEY_S:
            beginEdit(*appState, EditTarget::SeparationWeight);
            break;
        case GLFW_KEY_A:
            beginEdit(*appState, EditTarget::AlignmentWeight);
            break;
        case GLFW_KEY_C:
            beginEdit(*appState, EditTarget::CohesionWeight);
            break;
        default:
            break;
        }
    }

    void charCallback(GLFWwindow *window, unsigned int codepoint)
    {
        auto *appState = static_cast<AppState *>(glfwGetWindowUserPointer(window));

        if (appState == nullptr || appState->editTarget == EditTarget::None)
        {
            return;
        }

        const char character = static_cast<char>(codepoint);
        const bool allowDecimal = appState->editTarget != EditTarget::BoidCount;
        const bool isDigit = std::isdigit(static_cast<unsigned char>(character)) != 0;

        if (isDigit)
        {
            appState->editBuffer.push_back(character);
            logEditBuffer(*appState);
            return;
        }

        if (allowDecimal && character == '.' && appState->editBuffer.find('.') == std::string::npos)
        {
            if (appState->editBuffer.empty())
            {
                appState->editBuffer.push_back('0');
            }

            appState->editBuffer.push_back(character);
            logEditBuffer(*appState);
        }
    }

    void framebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        auto *appState = static_cast<AppState *>(glfwGetWindowUserPointer(window));

        if (appState != nullptr)
        {
            updateFramebufferSize(*appState, width, height);
            return;
        }

        glViewport(0, 0, std::max(width, 1), std::max(height, 1));
    }

    void renderFrame(const AppState &appState, const std::vector<float> &vertices)
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(appState.shaderProgram);
        glUniform2f(appState.windowSizeUniform,
                    static_cast<float>(WINDOW_WIDTH),
                    static_cast<float>(WINDOW_HEIGHT));

        glBindVertexArray(appState.vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, appState.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                     vertices.data(),
                     GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size() / 5));
    }

    void configureRendering(AppState &appState)
    {
        glGenVertexArrays(1, &appState.vertexArray);
        glGenBuffers(1, &appState.vertexBuffer);

        glBindVertexArray(appState.vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, appState.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * static_cast<GLsizei>(sizeof(float)), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * static_cast<GLsizei>(sizeof(float)), reinterpret_cast<void *>(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void destroyRendering(AppState &appState)
    {
        if (appState.vertexBuffer != 0)
        {
            glDeleteBuffers(1, &appState.vertexBuffer);
        }

        if (appState.vertexArray != 0)
        {
            glDeleteVertexArrays(1, &appState.vertexArray);
        }

        if (appState.shaderProgram != 0)
        {
            glDeleteProgram(appState.shaderProgram);
        }
    }
}

int main()
{
    OpenGLInitResult initResult = initializeOpenGL(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "Collective Behavior - Lab 10");

    if (!initResult.ok())
    {
        std::cout << initResult.errorMessage << "\n";
        return -1;
    }

    GLFWwindow *window = initResult.window;
    glfwSwapInterval(1);

    AppState appState;
    appState.emulator.initialize();
    appState.shaderProgram = createShaderProgram();

    if (appState.shaderProgram == 0)
    {
        shutdownOpenGL(window);
        return -1;
    }

    appState.windowSizeUniform = glGetUniformLocation(appState.shaderProgram, "uWindowSize");
    configureRendering(appState);

    glfwSetWindowUserPointer(window, &appState);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, charCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    updateFramebufferSize(appState, framebufferWidth, framebufferHeight);
    glClearColor(0.05f, 0.08f, 0.12f, 1.0f);

    std::cout << "Simulation controls: n/d/s/a/c then value then Enter. Escape cancels edits.\n";
    std::cout << "Initial parameters: " << appState.emulator.getParameterSummary() << "\n";
    std::cout << "Framebuffer size: " << appState.framebufferWidth << "x"
              << appState.framebufferHeight << "\n";

    DeltaTime::update();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        DeltaTime::update();
        const float deltaTime = std::min(DeltaTime::get(), 0.033f);

        appState.emulator.update(deltaTime);

        const std::vector<float> vertices = appState.emulator.buildRenderVertices();

        renderFrame(appState, vertices);

        glfwSwapBuffers(window);
    }

    destroyRendering(appState);
    shutdownOpenGL(window);
    return 0;
}
