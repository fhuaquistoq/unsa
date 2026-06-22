#pragma once

#include <string>

#include "core/EditorTypes.hpp"
#include "io/SceneSerializer.hpp"
#include "render/Renderer.hpp"

class EditorApp {
public:
    EditorApp(int argc, char** argv, std::string scenePath);
    int run();

private:
    static EditorApp* instance_;

    Scene scene_;
    EditorState state_;
    Renderer renderer_;
    std::string scenePath_;
    TransformMode mode_ = TransformMode::Translate;
    TransformAxis axis_ = TransformAxis::X;
    int width_ = 1280;
    int height_ = 720;
    int lastMouseX_ = 0;
    int lastMouseY_ = 0;
    bool leftDragging_ = false;
    bool rightDragging_ = false;
    bool transformedDuringDrag_ = false;

    void initializeGlut(int argc, char** argv);
    void initializeSceneFile();
    void createInitialScene();
    void saveScene(bool silent = false) const;
    void autosaveAfterObjectChange() const;
    void setMode(TransformMode mode);
    void setAxis(TransformAxis axis);
    void createObject(ObjectType type);
    bool transformSelected(int dx, int dy);
    void rotateCamera(int dx, int dy);
    void zoom(float amount);
    void focusSelected();
    void clearTerminal() const;
    void printMenu() const;
    void showLog(const std::string& message) const;
    void showSelectedObjectLog(const std::string& action, bool includeTransformContext = false) const;
    void showObjectLog(const std::string& action, const SceneObject& object, bool includeTransformContext = false) const;

    void display();
    void reshape(int width, int height);
    void keyboard(unsigned char key, int x, int y);
    void specialKeyboard(int key, int x, int y);
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);

    static void displayCallback();
    static void reshapeCallback(int width, int height);
    static void keyboardCallback(unsigned char key, int x, int y);
    static void specialKeyboardCallback(int key, int x, int y);
    static void mouseCallback(int button, int state, int x, int y);
    static void motionCallback(int x, int y);
};
