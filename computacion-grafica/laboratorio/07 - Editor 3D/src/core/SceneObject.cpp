#include "core/SceneObject.hpp"

std::string objectTypeToString(ObjectType type) {
    switch (type) {
        case ObjectType::Cube: return "cube";
        case ObjectType::Sphere: return "sphere";
        case ObjectType::Torus: return "torus";
        case ObjectType::Teapot: return "teapot";
    }
    return "cube";
}

bool objectTypeFromString(const std::string& value, ObjectType& type) {
    if (value == "cube") {
        type = ObjectType::Cube;
        return true;
    }
    if (value == "sphere") {
        type = ObjectType::Sphere;
        return true;
    }
    if (value == "torus") {
        type = ObjectType::Torus;
        return true;
    }
    if (value == "teapot") {
        type = ObjectType::Teapot;
        return true;
    }
    return false;
}
