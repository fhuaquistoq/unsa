#pragma once

#include <string>

#include "core/Math.hpp"

enum class ObjectType
{
    Cube,
    Sphere,
    Torus,
    Teapot
};

struct SceneObject
{
    int id = 0;
    ObjectType type = ObjectType::Cube;
    Vec3 position;
    Vec3 rotation;
    Vec3 scale = {1.0f, 1.0f, 1.0f};
    Vec3 color = {0.8f, 0.8f, 0.8f};
};

std::string objectTypeToString(ObjectType type);
bool objectTypeFromString(const std::string &value, ObjectType &type);
