#include "toolbar.h"
#include "imgui.h"

Toolbar::Toolbar() = default;

void Toolbar::render()
{
    // ─── Panel de herramientas ──────────────────────────────────
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 300), ImGuiCond_FirstUseEver);

    ImGui::Begin("Herramientas");

    // Selección de herramienta
    const char *tools[] = {"Brocha", u8"Línea", u8"Polígono"};
    int current = static_cast<int>(active_tool_);
    if (ImGui::Combo(u8"Imagen", &current, tools, IM_ARRAYSIZE(tools)))
    {
        active_tool_ = static_cast<Tool>(current);
    }

    ImGui::Separator();

    // Color picker
    color_changed_ = ImGui::ColorEdit3(u8"Color", brush_color_);

    ImGui::Separator();

    // Ajuste de lados del polígono (solo visible si herramienta activa es Polígono)
    if (active_tool_ == Tool::Polygon)
    {
        ImGui::Text(u8"Lados del polígono:");
        int prev = polygon_sides_;
        ImGui::SliderInt("##sides", &polygon_sides_, 3, 128);
        if (polygon_sides_ != prev)
        {
            // Se actualizó el número de lados
        }
    }

    ImGui::Separator();

    // Botón de limpiar canvas
    if (ImGui::Button(u8"Limpiar canvas"))
    {
        clear_requested_ = true;
    }

    ImGui::End();
}

void Toolbar::reset_flags()
{
    color_changed_ = false;
    clear_requested_ = false;
}