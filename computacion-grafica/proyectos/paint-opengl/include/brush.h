#pragma once
#include "shape.h"
#include <vector>

class Brush : public Shape
{
public:
    Brush();
    ~Brush() override = default;

    void add_point(float x, float y);
    void finish();

    void render(Renderer* renderer) const override;
    bool is_complete() const override;

private:
    // Pares (x, y) consecutivos: points_[0]=x0, points_[1]=y0, ...
    std::vector<float> points_;
    bool finished_ = false;
};