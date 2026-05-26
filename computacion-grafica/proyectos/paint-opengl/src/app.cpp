#include "app.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdio>
#include <cmath>

App::App() = default;

App::~App()
{
    cleanup();
}

bool App::init()
{
    // ─── GLFW ───────────────────────────────────────────────────
    if (!glfwInit())
    {
        std::fprintf(stderr, "Error: no se pudo inicializar GLFW\n");
        return false;
    }

    // OpenGL 3.3 Core Profile mínimo
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window_ = glfwCreateWindow(window_width_, window_height_,
                               "Paint OpenGL", nullptr, nullptr);
    if (!window_)
    {
        std::fprintf(stderr, "Error: no se pudo crear la ventana\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // VSync

    // ─── GLAD ──────────────────────────────────────────────────
    // GLAD1 usa gladLoadGLLoader() con el loader de GLFW.
    // GLAD2 usa gladLoadGL(glfwGetProcAddress) directamente.
    // Este proyecto usa GLAD1 (glad/glad.h), por lo tanto:
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::fprintf(stderr, "Error: no se pudo inicializar GLAD\n");
        return false;
    }

    std::fprintf(stdout, "OpenGL %s\n", glGetString(GL_VERSION));

    // ─── Callbacks ──────────────────────────────────────────────
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetCursorPosCallback(window_, cursor_callback);

    // ─── Canvas ─────────────────────────────────────────────────
    canvas_.init(window_width_, window_height_);

    // ─── ImGui ──────────────────────────────────────────────────
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

void App::run()
{
    while (!glfwWindowShouldClose(window_))
    {
        process_input();

        glfwPollEvents();

        // ─── Toolbar state changes ────────────────────────────────
        if (toolbar_.clear_requested())
        {
            canvas_.clear_shapes();
            canvas_.clear();
        }
        toolbar_.reset_flags();

        // ─── ImGui frame ────────────────────────────────────────
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        toolbar_.render();

        // ─── Render canvas ──────────────────────────────────────
        glViewport(0, 0, window_width_, window_height_);
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        canvas_.render(current_shape_.get());

        // ─── ImGui render ───────────────────────────────────────
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window_);
    }
}

void App::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window_)
    {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

void App::process_input()
{
    // TODO: manejar mouse para dibujar según herramienta activa
    // Esc para salir
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window_, true);
    }
}

void App::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    auto *self = static_cast<App *>(glfwGetWindowUserPointer(window));
    if (self)
    {
        self->window_width_ = width;
        self->window_height_ = height;
        self->canvas_.resize(width, height);
    }
}

void App::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    auto *self = static_cast<App *>(glfwGetWindowUserPointer(window));
    if (self)
    {
        self->on_mouse_button(button, action, mods);
    }
}

void App::cursor_callback(GLFWwindow *window, double xpos, double ypos)
{
    auto *self = static_cast<App *>(glfwGetWindowUserPointer(window));
    if (self)
    {
        self->on_mouse_move(xpos, ypos);
    }
}

void App::on_mouse_button(int button, int action, int mods)
{
    // Solo manejar botón izquierdo
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    // Si ImGui quiere el mouse, no procesar
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    // Convertir coordenadas de pantalla (origen top-left) a OpenGL (origen bottom-left)
    float x = static_cast<float>(mouse_x_);
    float y = static_cast<float>(window_height_) - static_cast<float>(mouse_y_);

    if (action == GLFW_PRESS)
    {
        mouse_pressed_ = true;

        switch (toolbar_.active_tool())
        {
        case Tool::Brush:
            current_shape_ = std::make_unique<Brush>();
            current_shape_->color.r = toolbar_.brush_color_r();
            current_shape_->color.g = toolbar_.brush_color_g();
            current_shape_->color.b = toolbar_.brush_color_b();
            dynamic_cast<Brush *>(current_shape_.get())->add_point(x, y);
            break;
        case Tool::Line:
            current_shape_ = std::make_unique<Line>();
            current_shape_->color.r = toolbar_.brush_color_r();
            current_shape_->color.g = toolbar_.brush_color_g();
            current_shape_->color.b = toolbar_.brush_color_b();
            dynamic_cast<Line *>(current_shape_.get())->set_start(x, y);
            break;
        case Tool::Polygon:
            current_shape_ = std::make_unique<Polygon>(toolbar_.polygon_sides());
            current_shape_->color.r = toolbar_.brush_color_r();
            current_shape_->color.g = toolbar_.brush_color_g();
            current_shape_->color.b = toolbar_.brush_color_b();
            dynamic_cast<Polygon *>(current_shape_.get())->set_center(x, y);
            break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        mouse_pressed_ = false;

        if (!current_shape_)
            return;

        switch (toolbar_.active_tool())
        {
        case Tool::Brush:
            dynamic_cast<Brush *>(current_shape_.get())->finish();
            break;
        case Tool::Line:
            dynamic_cast<Line *>(current_shape_.get())->set_end(x, y);
            break;
        case Tool::Polygon:
        {
            auto *poly = dynamic_cast<Polygon *>(current_shape_.get());
            float radius = std::sqrt((x - poly->center_x()) * (x - poly->center_x()) +
                                    (y - poly->center_y()) * (y - poly->center_y()));
            poly->set_radius(radius);
            break;
        }
        }

        if (current_shape_->is_complete())
        {
            canvas_.add_shape(std::move(current_shape_));
        }
        current_shape_.reset();
    }
}

void App::on_mouse_move(double xpos, double ypos)
{
    mouse_x_ = static_cast<float>(xpos);
    mouse_y_ = static_cast<float>(ypos);

    // Convertir coordenadas de pantalla a OpenGL
    float x = static_cast<float>(xpos);
    float y = static_cast<float>(window_height_) - static_cast<float>(ypos);

    if (mouse_pressed_ && current_shape_)
    {
        switch (toolbar_.active_tool())
        {
        case Tool::Brush:
            dynamic_cast<Brush *>(current_shape_.get())->add_point(x, y);
            break;
        case Tool::Line:
            dynamic_cast<Line *>(current_shape_.get())->set_end(x, y);
            break;
        case Tool::Polygon:
        {
            auto *poly = dynamic_cast<Polygon *>(current_shape_.get());
            float radius = std::sqrt((x - poly->center_x()) * (x - poly->center_x()) +
                                    (y - poly->center_y()) * (y - poly->center_y()));
            poly->set_radius(radius);
            break;
        }
        }
    }
}