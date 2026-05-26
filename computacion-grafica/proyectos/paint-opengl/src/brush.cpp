#include "brush.h"
#include "renderer.h"
#include <vector>

Brush::Brush() = default;

void Brush::add_point(float x, float y)
{
    points_.push_back(x);
    points_.push_back(y);
}

void Brush::finish()
{
    finished_ = true;
}

void Brush::render(Renderer* renderer) const
{
    if (points_.size() < 4)
        return; // Necesitamos al menos 2 puntos

    renderer->draw_line_strip(points_, color);
}

bool Brush::is_complete() const
{
    return finished_ && points_.size() >= 4;
}