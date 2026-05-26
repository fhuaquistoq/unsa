#include "line.h"
#include "renderer.h"
#include <vector>

Line::Line() = default;

void Line::set_start(float x, float y)
{
    start_x_ = x;
    start_y_ = y;
    has_start_ = true;
}

void Line::set_end(float x, float y)
{
    end_x_ = x;
    end_y_ = y;
}

void Line::render(Renderer* renderer) const
{
    if (!has_start_)
        return;
    std::vector<float> vertices = { start_x_, start_y_, end_x_, end_y_ };
    renderer->draw_lines(vertices, color);
}

bool Line::is_complete() const
{
    return has_start_;
}