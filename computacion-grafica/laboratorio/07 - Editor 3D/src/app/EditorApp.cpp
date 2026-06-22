#include "app/EditorApp.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <GL/glut.h>

static constexpr float kPi = 3.1415926535f;
static constexpr float kProjectionEpsilon = 0.001f;

static float radians(float degrees) {
    return degrees * kPi / 180.0f;
}

static std::string modeName(TransformMode mode) {
    switch (mode) {
        case TransformMode::Translate: return "Translate";
        case TransformMode::Rotate: return "Rotate";
        case TransformMode::Scale: return "Scale";
    }
    return "Unknown";
}

static std::string axisName(TransformAxis axis) {
    switch (axis) {
        case TransformAxis::X: return "X";
        case TransformAxis::Y: return "Y";
        case TransformAxis::Z: return "Z";
    }
    return "?";
}

static float dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static Vec3 normalized(const Vec3& value) {
    const float length = std::sqrt(dot(value, value));
    if (length <= kProjectionEpsilon) {
        return {0.0f, 0.0f, 0.0f};
    }
    return value * (1.0f / length);
}

static Vec3 worldAxis(TransformAxis axis) {
    switch (axis) {
        case TransformAxis::X: return {1.0f, 0.0f, 0.0f};
        case TransformAxis::Y: return {0.0f, 1.0f, 0.0f};
        case TransformAxis::Z: return {0.0f, 0.0f, 1.0f};
    }
    return {0.0f, 0.0f, 0.0f};
}

static float dragAmountForAxis(TransformAxis axis, int dx, int dy) {
    switch (axis) {
        case TransformAxis::X: return static_cast<float>(dx);
        case TransformAxis::Y: return static_cast<float>(-dy);
        case TransformAxis::Z: return static_cast<float>(dx - dy);
    }
    return 0.0f;
}

static float screenProjectedDragAmount(const CameraState& camera, TransformAxis axis, int dx, int dy) {
    const float yaw = radians(camera.yaw);
    const float pitch = radians(camera.pitch);
    const Vec3 eye{
        camera.target.x + camera.distance * std::cos(pitch) * std::sin(yaw),
        camera.target.y + camera.distance * std::sin(pitch),
        camera.target.z + camera.distance * std::cos(pitch) * std::cos(yaw)
    };
    const Vec3 forward = normalized(camera.target - eye);
    const Vec3 right = normalized(cross(forward, {0.0f, 1.0f, 0.0f}));
    const Vec3 up = normalized(cross(right, forward));
    const Vec3 activeAxis = worldAxis(axis);

    const float screenX = dot(activeAxis, right);
    const float screenY = dot(activeAxis, up);
    const float screenLength = std::sqrt(screenX * screenX + screenY * screenY);
    if (screenLength <= kProjectionEpsilon) {
        return dragAmountForAxis(axis, dx, dy);
    }

    return (static_cast<float>(dx) * screenX + static_cast<float>(-dy) * screenY) / screenLength;
}

EditorApp* EditorApp::instance_ = nullptr;

EditorApp::EditorApp(int argc, char** argv, std::string scenePath) : scenePath_(std::move(scenePath)) {
    EditorApp::instance_ = this;
    this->initializeGlut(argc, argv);
    this->initializeSceneFile();
}

int EditorApp::run() {
    glutMainLoop();
    return 0;
}

void EditorApp::initializeGlut(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(this->width_, this->height_);
    glutCreateWindow("Editor 3D - OpenGL Lab");
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutKeyboardFunc(keyboardCallback);
    glutSpecialFunc(specialKeyboardCallback);
    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
}

void EditorApp::initializeSceneFile() {
    if (std::filesystem::exists(this->scenePath_)) {
        if (SceneSerializer::load(this->scenePath_, this->scene_, this->state_)) {
            this->showSelectedObjectLog("loaded scene");
            return;
        }
        this->showLog("Could not load scene. Creating a new one.");
    }

    this->createInitialScene();
    this->saveScene(true);
    this->showSelectedObjectLog("initial scene created");
}

void EditorApp::createInitialScene() {
    this->scene_.clear();
    SceneObject& cube = this->scene_.addObject(ObjectType::Cube);
    cube.position = {0.0f, 0.5f, 0.0f};
    cube.color = {0.30f, 0.55f, 0.95f};
}

void EditorApp::saveScene(bool silent) const {
    if (SceneSerializer::save(this->scenePath_, this->scene_, this->state_)) {
        if (!silent) {
            this->showLog("Scene saved: " + this->scenePath_);
        }
        return;
    }
    this->showLog("ERROR: scene could not be saved: " + this->scenePath_);
}

void EditorApp::autosaveAfterObjectChange() const {
    this->saveScene(true);
}

void EditorApp::setMode(TransformMode mode) {
    this->mode_ = mode;
    this->showLog("Mode: " + modeName(this->mode_) + " | Axis: " + axisName(this->axis_));
}

void EditorApp::setAxis(TransformAxis axis) {
    this->axis_ = axis;
    this->showLog("Mode: " + modeName(this->mode_) + " | Axis: " + axisName(this->axis_));
}

void EditorApp::createObject(ObjectType type) {
    SceneObject& object = this->scene_.addObject(type);
    object.position.y = 0.5f;
    this->autosaveAfterObjectChange();
    this->showSelectedObjectLog("created");
    glutPostRedisplay();
}

bool EditorApp::transformSelected(int dx, int dy) {
    SceneObject* object = this->scene_.selectedObject();
    if (!object) {
        return false;
    }

    if (dx == 0 && dy == 0) {
        return false;
    }

    switch (this->mode_) {
        case TransformMode::Translate: {
            const float amount = screenProjectedDragAmount(this->state_.camera, this->axis_, dx, dy) * 0.025f;
            if (this->axis_ == TransformAxis::X) object->position.x += amount;
            if (this->axis_ == TransformAxis::Y) object->position.y += amount;
            if (this->axis_ == TransformAxis::Z) object->position.z += amount;
            break;
        }
        case TransformMode::Rotate: {
            const float amount = dragAmountForAxis(this->axis_, dx, dy) * 0.6f;
            if (this->axis_ == TransformAxis::X) object->rotation.x += amount;
            if (this->axis_ == TransformAxis::Y) object->rotation.y += amount;
            if (this->axis_ == TransformAxis::Z) object->rotation.z += amount;
            break;
        }
        case TransformMode::Scale: {
            const float delta = 1.0f + static_cast<float>(dx - dy) * 0.008f;
            const float factor = clampFloat(delta, 0.85f, 1.15f);
            object->scale.x = clampFloat(object->scale.x * factor, 0.1f, 8.0f);
            object->scale.y = clampFloat(object->scale.y * factor, 0.1f, 8.0f);
            object->scale.z = clampFloat(object->scale.z * factor, 0.1f, 8.0f);
            break;
        }
    }
    return true;
}

void EditorApp::rotateCamera(int dx, int dy) {
    this->state_.camera.yaw += static_cast<float>(dx) * 0.35f;
    this->state_.camera.pitch = clampFloat(this->state_.camera.pitch + static_cast<float>(dy) * 0.35f, -85.0f, 85.0f);
}

void EditorApp::zoom(float amount) {
    this->state_.camera.distance = clampFloat(this->state_.camera.distance + amount, 2.0f, 80.0f);
    this->showLog("Camera zoom distance: " + std::to_string(this->state_.camera.distance));
}

void EditorApp::focusSelected() {
    const SceneObject* object = this->scene_.selectedObject();
    if (!object) {
        return;
    }
    this->state_.camera.target = object->position;
    this->showLog("Camera focused on selected object.");
    glutPostRedisplay();
}

void EditorApp::clearTerminal() const {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H";
#endif
}

void EditorApp::printMenu() const {
    std::cout << "3D Editor - Controls\n"
              << "Scene: " << this->scenePath_ << "\n\n"
              << "Objects: 1 cube | 2 sphere | 3 torus | 4 teapot | d duplicate | Delete/Backspace delete\n"
              << "Selection: Tab next | Shift+Tab previous | Home previous | End next\n"
              << "Transform: t translate | r rotate | s scale | x/y/z axis | left drag apply\n"
              << "Camera: right drag rotate | mouse wheel or +/- zoom | f focus | arrows pan | PageUp/PageDown vertical\n"
              << "Projection/View: [/] FOV | n/m near plane | ,/. far plane | w wireframe\n"
              << "File: Ctrl+S or F1 manual save | Esc save and exit\n"
              << "Autosave: object changes only; camera/projection/view changes do not autosave.\n\n";
}

void EditorApp::showLog(const std::string& message) const {
    this->clearTerminal();
    this->printMenu();
    std::cout << message << std::endl;
}

void EditorApp::showSelectedObjectLog(const std::string& action, bool includeTransformContext) const {
    const SceneObject* object = this->scene_.selectedObject();
    if (!object) {
        this->showLog(action + ": no selected object");
        return;
    }

    this->showObjectLog(action, *object, includeTransformContext);
}

void EditorApp::showObjectLog(const std::string& action, const SceneObject& object, bool includeTransformContext) const {
    std::ostringstream message;
    message << std::fixed << std::setprecision(2)
            << "[" << action << "] id=" << object.id
            << " type=" << objectTypeToString(object.type)
            << " position=(" << object.position.x << ", " << object.position.y << ", " << object.position.z << ")"
            << " rotation=(" << object.rotation.x << ", " << object.rotation.y << ", " << object.rotation.z << ")"
            << " scale=(" << object.scale.x << ", " << object.scale.y << ", " << object.scale.z << ")"
            << " color=(" << object.color.x << ", " << object.color.y << ", " << object.color.z << ")";
    if (includeTransformContext) {
        message << " mode=" << modeName(this->mode_) << " axis=" << axisName(this->axis_);
    }
    this->showLog(message.str());
}

void EditorApp::display() {
    this->renderer_.render(this->scene_, this->state_, {this->mode_, this->axis_}, this->width_, this->height_);
}

void EditorApp::reshape(int width, int height) {
    this->width_ = std::max(1, width);
    this->height_ = std::max(1, height);
    glutPostRedisplay();
}

void EditorApp::keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: this->saveScene(); std::exit(0);
        case 8:
        case 127: {
            const SceneObject* selected = this->scene_.selectedObject();
            if (selected) {
                const SceneObject deleted = *selected;
                this->scene_.deleteSelected();
                this->autosaveAfterObjectChange();
                this->showObjectLog("deleted", deleted);
            } else {
                this->showLog("delete: no selected object");
            }
            break;
        }
        case 9: this->scene_.selectNext(); this->showSelectedObjectLog("selected"); break;
        case 25: this->scene_.selectPrevious(); this->showSelectedObjectLog("selected"); break;
        case 19: this->saveScene(); break;
        case '1': this->createObject(ObjectType::Cube); break;
        case '2': this->createObject(ObjectType::Sphere); break;
        case '3': this->createObject(ObjectType::Torus); break;
        case '4': this->createObject(ObjectType::Teapot); break;
        case 't': case 'T': this->setMode(TransformMode::Translate); break;
        case 'r': case 'R': this->setMode(TransformMode::Rotate); break;
        case 's': this->setMode(TransformMode::Scale); break;
        case 'x': case 'X': this->setAxis(TransformAxis::X); break;
        case 'y': case 'Y': this->setAxis(TransformAxis::Y); break;
        case 'z': case 'Z': this->setAxis(TransformAxis::Z); break;
        case 'w': case 'W': this->state_.wireframe = !this->state_.wireframe; this->showLog(this->state_.wireframe ? "Wireframe enabled." : "Wireframe disabled."); break;
        case 'd': case 'D': {
            if (this->scene_.selectedObject()) {
                this->scene_.duplicateSelected();
                this->autosaveAfterObjectChange();
                this->showSelectedObjectLog("duplicated");
            } else {
                this->showLog("duplicate: no selected object");
            }
            break;
        }
        case 'f': case 'F': this->focusSelected(); break;
        case '+': this->zoom(-0.5f); break;
        case '-': this->zoom(0.5f); break;
        case '[': this->state_.projection.fov = clampFloat(this->state_.projection.fov - 2.0f, 20.0f, 120.0f); this->showLog("FOV: " + std::to_string(this->state_.projection.fov)); break;
        case ']': this->state_.projection.fov = clampFloat(this->state_.projection.fov + 2.0f, 20.0f, 120.0f); this->showLog("FOV: " + std::to_string(this->state_.projection.fov)); break;
        case 'n': this->state_.projection.nearPlane = clampFloat(this->state_.projection.nearPlane - 0.05f, 0.05f, this->state_.projection.farPlane - 1.0f); this->showLog("Near plane: " + std::to_string(this->state_.projection.nearPlane)); break;
        case 'm': this->state_.projection.nearPlane = clampFloat(this->state_.projection.nearPlane + 0.05f, 0.05f, this->state_.projection.farPlane - 1.0f); this->showLog("Near plane: " + std::to_string(this->state_.projection.nearPlane)); break;
        case ',': this->state_.projection.farPlane = clampFloat(this->state_.projection.farPlane - 5.0f, this->state_.projection.nearPlane + 1.0f, 500.0f); this->showLog("Far plane: " + std::to_string(this->state_.projection.farPlane)); break;
        case '.': this->state_.projection.farPlane = clampFloat(this->state_.projection.farPlane + 5.0f, this->state_.projection.nearPlane + 1.0f, 500.0f); this->showLog("Far plane: " + std::to_string(this->state_.projection.farPlane)); break;
    }
    glutPostRedisplay();
}

void EditorApp::specialKeyboard(int key, int, int) {
    const float step = 0.4f;
    const float yaw = radians(this->state_.camera.yaw);
    const Vec3 forward{-std::sin(yaw), 0.0f, -std::cos(yaw)};
    const Vec3 right{std::cos(yaw), 0.0f, -std::sin(yaw)};

    switch (key) {
        case GLUT_KEY_LEFT: this->state_.camera.target = this->state_.camera.target - right * step; break;
        case GLUT_KEY_RIGHT: this->state_.camera.target = this->state_.camera.target + right * step; break;
        case GLUT_KEY_UP: this->state_.camera.target = this->state_.camera.target + forward * step; break;
        case GLUT_KEY_DOWN: this->state_.camera.target = this->state_.camera.target - forward * step; break;
        case GLUT_KEY_PAGE_UP: this->state_.camera.target.y += step; break;
        case GLUT_KEY_PAGE_DOWN: this->state_.camera.target.y -= step; break;
        case GLUT_KEY_HOME: this->scene_.selectPrevious(); this->showSelectedObjectLog("selected"); break;
        case GLUT_KEY_END: this->scene_.selectNext(); this->showSelectedObjectLog("selected"); break;
        case GLUT_KEY_F1: this->saveScene(); break;
        default: break;
    }
    glutPostRedisplay();
}

void EditorApp::mouse(int button, int state, int x, int y) {
    this->lastMouseX_ = x;
    this->lastMouseY_ = y;

    if (button == 3 && state == GLUT_DOWN) {
        this->zoom(-0.5f);
        glutPostRedisplay();
        return;
    }
    if (button == 4 && state == GLUT_DOWN) {
        this->zoom(0.5f);
        glutPostRedisplay();
        return;
    }

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            this->leftDragging_ = true;
            this->transformedDuringDrag_ = false;
        } else {
            this->leftDragging_ = false;
            if (this->transformedDuringDrag_) {
                this->autosaveAfterObjectChange();
                this->showSelectedObjectLog("updated", true);
                this->transformedDuringDrag_ = false;
            }
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        this->rightDragging_ = state == GLUT_DOWN;
    }
}

void EditorApp::motion(int x, int y) {
    const int dx = x - this->lastMouseX_;
    const int dy = y - this->lastMouseY_;
    this->lastMouseX_ = x;
    this->lastMouseY_ = y;

    if (this->leftDragging_) {
        this->transformedDuringDrag_ = this->transformSelected(dx, dy) || this->transformedDuringDrag_;
    } else if (this->rightDragging_) {
        this->rotateCamera(dx, dy);
    }
    glutPostRedisplay();
}

void EditorApp::displayCallback() { EditorApp::instance_->display(); }
void EditorApp::reshapeCallback(int width, int height) { EditorApp::instance_->reshape(width, height); }
void EditorApp::keyboardCallback(unsigned char key, int x, int y) { EditorApp::instance_->keyboard(key, x, y); }
void EditorApp::specialKeyboardCallback(int key, int x, int y) { EditorApp::instance_->specialKeyboard(key, x, y); }
void EditorApp::mouseCallback(int button, int state, int x, int y) { EditorApp::instance_->mouse(button, state, x, y); }
void EditorApp::motionCallback(int x, int y) { EditorApp::instance_->motion(x, y); }
