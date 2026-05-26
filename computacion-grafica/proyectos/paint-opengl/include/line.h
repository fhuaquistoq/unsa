#pragma once
#include "shape.h"

class Line : public Shape
{
public:
    Line();
    ~Line() override = default;

    void set_start(float x, float y);
    void set_end(float x, float y);

    void render(Renderer* renderer) const override;
    bool is_complete() const override;

private:
    float start_x_ = 0.0f, start_y_ = 0.0f;
    float end_x_ = 0.0f, end_y_ = 0.0f;
    bool has_start_ = false;
};