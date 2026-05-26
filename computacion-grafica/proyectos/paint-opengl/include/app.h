#pragma once
#include <glad/glad.h>
#include <memory>
#include "canvas.h"
#include "toolbar.h"
#include "line.h"
#include "polygon.h"
#include "brush.h"

struct GLFWwindow;

// Aplicación principal: gestión de ventana, input y loop de render.
// Encargada de coordinar Canvas + Toolbar.
class App
{
public:
    App();
    ~App();

    bool init();    // Crea ventana, inicializa GLFW/GLAD/ImGui
    void run();     // Loop principal
    void cleanup(); // Libera recursos

private:
    GLFWwindow *window_ = nullptr;
    Canvas canvas_;
    Toolbar toolbar_;

    int window_width_ = 1280;
    int window_height_ = 720;

    // Estado del mouse
    bool mouse_pressed_ = false;
    float mouse_x_ = 0.0f;
    float mouse_y_ = 0.0f;

    // Figura en progreso (preview mientras se arrastra el mouse)
    std::unique_ptr<Shape> current_shape_;

    void process_input();
    void on_mouse_button(int button, int action, int mods);
    void on_mouse_move(double xpos, double ypos);

    // Callbacks estáticos para GLFW
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
    static void cursor_callback(GLFWwindow *window, double xpos, double ypos);
};