#include "polygon.h"
#include "renderer.h"
#include <cmath>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Polygon::Polygon(int sides) : sides_(sides) {}

void Polygon::set_center(float x, float y)
{
    center_x_ = x;
    center_y_ = y;
    has_center_ = true;
}

void Polygon::set_radius(float radius)
{
    radius_ = radius;
}

void Polygon::set_sides(int sides)
{
    sides_ = sides < 3 ? 3 : sides;
}

void Polygon::render(Renderer* renderer) const
{
    if (!has_center_ || radius_ <= 0.0f)
        return;

    // Generar vértices del polígono regular
    std::vector<float> outline;
    outline.reserve(static_cast<size_t>(sides_) * 2 + 2);

    float angle_step = 2.0f * static_cast<float>(M_PI) / static_cast<float>(sides_);
    for (int i = 0; i < sides_; ++i)
    {
        float angle = angle_step * static_cast<float>(i);
        outline.push_back(center_x_ + radius_ * std::cos(angle));
        outline.push_back(center_y_ + radius_ * std::sin(angle));
    }
    // Cerrar el outline
    outline.push_back(outline[0]);
    outline.push_back(outline[1]);

    // Triangular como fan desde el centro
    std::vector<float> triangles;
    triangles.reserve(static_cast<size_t>(sides_) * 6);
    for (int i = 0; i < sides_; ++i)
    {
        int next = (i + 1) % sides_;
        // centro, v_i, v_next
        triangles.push_back(center_x_);
        triangles.push_back(center_y_);
        triangles.push_back(outline[i * 2]);
        triangles.push_back(outline[i * 2 + 1]);
        triangles.push_back(outline[next * 2]);
        triangles.push_back(outline[next * 2 + 1]);
    }

    renderer->draw_triangles(triangles, color);
}

bool Polygon::is_complete() const
{
    return has_center_ && radius_ > 0.0f;
}