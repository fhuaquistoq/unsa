#pragma once

// Enumeración de herramientas disponibles
enum class Tool
{
    Line,
    Polygon,
    Brush
};

// Toolbar: panel ImGui con selección de herramienta,
// color picker y ajuste de lados del polígono.
class Toolbar
{
public:
    Toolbar();
    ~Toolbar() = default;

    void render(); // Dibuja el panel ImGui

    // Estado actual (leído por App/Canvas)
    Tool active_tool() const { return active_tool_; }
    bool color_changed() const { return color_changed_; }
    float brush_color_r() const { return brush_color_[0]; }
    float brush_color_g() const { return brush_color_[1]; }
    float brush_color_b() const { return brush_color_[2]; }
    int polygon_sides() const { return polygon_sides_; }
    bool clear_requested() const { return clear_requested_; }

    void reset_flags();

private:
    Tool active_tool_ = Tool::Brush;
    float brush_color_[3] = {1.0f, 1.0f, 1.0f};
    int polygon_sides_ = 3;
    bool color_changed_ = false;
    bool clear_requested_ = false;
};