#pragma once

#include <string>

#include "core/Scene.hpp"

struct CameraState {
    float yaw = 35.0f;
    float pitch = 25.0f;
    float distance = 10.0f;
    Vec3 target = {0.0f, 0.0f, 0.0f};
};

struct ProjectionState {
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};

struct EditorState {
    CameraState camera;
    ProjectionState projection;
    bool wireframe = false;
};

class SceneSerializer {
public:
    static bool load(const std::string& path, Scene& scene, EditorState& state);
    static bool save(const std::string& path, const Scene& scene, const EditorState& state);
};
