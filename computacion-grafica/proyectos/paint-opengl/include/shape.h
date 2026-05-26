#pragma once

class Renderer; // forward declaration

// Color RGBA con valores normalizados [0.0, 1.0]
struct Color
{
    float r, g, b, a;

    Color(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
        : r(r), g(g), b(b), a(a) {}
};

// Clase base abstracta para todas las figuras dibujables.
// Cada figura sabe renderizarse y sabe si está completa.
class Shape
{
public:
    virtual ~Shape() = default;

    virtual void render(Renderer* renderer) const = 0;
    virtual bool is_complete() const = 0;

    Color color{1.0f, 1.0f, 1.0f, 1.0f};
};