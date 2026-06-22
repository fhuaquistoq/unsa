#pragma once

#include "core/Scene.hpp"
#include "core/EditorTypes.hpp"
#include "io/SceneSerializer.hpp"

struct RenderUiState {
    TransformMode mode = TransformMode::Translate;
    TransformAxis axis = TransformAxis::X;
};

class Renderer {
public:
    void render(const Scene& scene, const EditorState& state, const RenderUiState& ui, int width, int height) const;

private:
    void setupCamera(const CameraState& camera, const ProjectionState& projection, int width, int height) const;
    void drawGrid() const;
    void drawWorldAxes() const;
    void drawObject(const SceneObject& object, bool selected, bool wireframe) const;
    void drawSelectionOutline(const SceneObject& object) const;
    void drawLocalGizmo(const SceneObject& object, TransformMode mode, TransformAxis axis) const;
    void drawCornerGizmo(const CameraState& camera, int width, int height) const;
};
