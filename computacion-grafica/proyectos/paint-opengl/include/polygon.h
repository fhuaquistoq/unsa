#pragma once
#include "shape.h"

class Polygon : public Shape
{
public:
    // N lados mínimo = 3 (triángulo), puede crecer "infinitamente"
    explicit Polygon(int sides = 3);
    ~Polygon() override = default;

    void set_center(float x, float y);
    void set_radius(float radius);
    void set_sides(int sides);

    int get_sides() const { return sides_; }
    float center_x() const { return center_x_; }
    float center_y() const { return center_y_; }
    void render(Renderer* renderer) const override;
    bool is_complete() const override;

private:
    float center_x_ = 0.0f, center_y_ = 0.0f;
    float radius_ = 0.0f;
    int sides_;
    bool has_center_ = false;
};