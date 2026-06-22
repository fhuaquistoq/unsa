#include "render/Renderer.hpp"

#include <cmath>

#include <GL/glut.h>

static constexpr float kPi = 3.1415926535f;

static float radians(float degrees) {
    return degrees * kPi / 180.0f;
}

static void drawAxis(float length, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(length, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glutSolidCone(0.06, 0.18, 12, 4);
    glPopMatrix();
}

static bool isActiveAxis(TransformAxis active, TransformAxis axis) {
    return active == axis;
}

static void axisColor(TransformAxis active, TransformAxis axis, float r, float g, float b) {
    if (isActiveAxis(active, axis)) {
        glColor3f(1.0f, 0.95f, 0.2f);
    } else {
        glColor3f(r, g, b);
    }
}

static void drawRing(float radius) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 64; ++i) {
        const float angle = static_cast<float>(i) * 2.0f * kPi / 64.0f;
        glVertex3f(std::cos(angle) * radius, std::sin(angle) * radius, 0.0f);
    }
    glEnd();
}

void Renderer::render(const Scene& scene, const EditorState& state, const RenderUiState& ui, int width, int height) const {
    glViewport(0, 0, width, height);
    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->setupCamera(state.camera, state.projection, width, height);
    this->drawGrid();
    this->drawWorldAxes();

    glPolygonMode(GL_FRONT_AND_BACK, state.wireframe ? GL_LINE : GL_FILL);
    for (const SceneObject& object : scene.objects()) {
        this->drawObject(object, object.id == scene.selectedId(), state.wireframe);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (const SceneObject* selected = scene.selectedObject()) {
        if (ui.mode == TransformMode::Translate || ui.mode == TransformMode::Rotate) {
            this->drawLocalGizmo(*selected, ui.mode, ui.axis);
        }
    }

    this->drawCornerGizmo(state.camera, width, height);
    glutSwapBuffers();
}

void Renderer::setupCamera(const CameraState& camera, const ProjectionState& projection, int width, int height) const {
    const float aspect = height == 0 ? 1.0f : static_cast<float>(width) / static_cast<float>(height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(projection.fov, aspect, projection.nearPlane, projection.farPlane);

    const float yaw = radians(camera.yaw);
    const float pitch = radians(camera.pitch);
    const float x = camera.target.x + camera.distance * std::cos(pitch) * std::sin(yaw);
    const float y = camera.target.y + camera.distance * std::sin(pitch);
    const float z = camera.target.z + camera.distance * std::cos(pitch) * std::cos(yaw);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z, camera.target.x, camera.target.y, camera.target.z, 0.0f, 1.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    GLfloat lightPosition[] = {4.0f, 6.0f, 6.0f, 1.0f};
    GLfloat ambient[] = {0.25f, 0.25f, 0.25f, 1.0f};
    GLfloat diffuse[] = {0.85f, 0.85f, 0.85f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}

void Renderer::drawGrid() const {
    glDisable(GL_LIGHTING);
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_LINES);
    for (int i = -10; i <= 10; ++i) {
        glVertex3f(static_cast<float>(i), 0.0f, -10.0f);
        glVertex3f(static_cast<float>(i), 0.0f, 10.0f);
        glVertex3f(-10.0f, 0.0f, static_cast<float>(i));
        glVertex3f(10.0f, 0.0f, static_cast<float>(i));
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void Renderer::drawWorldAxes() const {
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.1f, 0.1f);
    glVertex3f(0.0f, 0.02f, 0.0f);
    glVertex3f(2.5f, 0.02f, 0.0f);
    glColor3f(0.1f, 1.0f, 0.1f);
    glVertex3f(0.0f, 0.02f, 0.0f);
    glVertex3f(0.0f, 2.5f, 0.0f);
    glColor3f(0.1f, 0.3f, 1.0f);
    glVertex3f(0.0f, 0.02f, 0.0f);
    glVertex3f(0.0f, 0.02f, 2.5f);
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void Renderer::drawObject(const SceneObject& object, bool selected, bool wireframe) const {
    glPushMatrix();
    glTranslatef(object.position.x, object.position.y, object.position.z);
    glRotatef(object.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(object.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(object.rotation.z, 0.0f, 0.0f, 1.0f);
    glScalef(object.scale.x, object.scale.y, object.scale.z);
    glColor3f(
        selected ? std::min(1.0f, object.color.x + 0.25f) : object.color.x,
        selected ? std::min(1.0f, object.color.y + 0.15f) : object.color.y,
        selected ? std::max(0.0f, object.color.z - 0.10f) : object.color.z
    );

    switch (object.type) {
        case ObjectType::Cube: glutSolidCube(1.0); break;
        case ObjectType::Sphere: glutSolidSphere(0.55, 32, 16); break;
        case ObjectType::Torus: glutSolidTorus(0.18, 0.45, 24, 36); break;
        case ObjectType::Teapot: glutSolidTeapot(0.55); break;
    }
    glPopMatrix();

    if (selected && !wireframe) {
        this->drawSelectionOutline(object);
    }
}

void Renderer::drawSelectionOutline(const SceneObject& object) const {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.55f, 0.0f);
    glLineWidth(2.0f);
    glPushMatrix();
    glTranslatef(object.position.x, object.position.y, object.position.z);
    glRotatef(object.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(object.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(object.rotation.z, 0.0f, 0.0f, 1.0f);
    glScalef(object.scale.x * 1.12f, object.scale.y * 1.12f, object.scale.z * 1.12f);
    glutWireCube(1.15);
    glPopMatrix();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void Renderer::drawLocalGizmo(const SceneObject& object, TransformMode mode, TransformAxis axis) const {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(3.0f);
    glPushMatrix();
    glTranslatef(object.position.x, object.position.y, object.position.z);
    glRotatef(object.rotation.x, 1.0f, 0.0f, 0.0f);
    glRotatef(object.rotation.y, 0.0f, 1.0f, 0.0f);
    glRotatef(object.rotation.z, 0.0f, 0.0f, 1.0f);

    if (mode == TransformMode::Translate) {
        axisColor(axis, TransformAxis::X, 1.0f, 0.1f, 0.1f);
        drawAxis(1.3f, isActiveAxis(axis, TransformAxis::X) ? 1.0f : 1.0f, isActiveAxis(axis, TransformAxis::X) ? 0.95f : 0.1f, isActiveAxis(axis, TransformAxis::X) ? 0.2f : 0.1f);
        glPushMatrix();
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
        drawAxis(1.3f, isActiveAxis(axis, TransformAxis::Y) ? 1.0f : 0.1f, isActiveAxis(axis, TransformAxis::Y) ? 0.95f : 1.0f, isActiveAxis(axis, TransformAxis::Y) ? 0.2f : 0.1f);
        glPopMatrix();
        glPushMatrix();
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        drawAxis(1.3f, isActiveAxis(axis, TransformAxis::Z) ? 1.0f : 0.1f, isActiveAxis(axis, TransformAxis::Z) ? 0.95f : 0.35f, isActiveAxis(axis, TransformAxis::Z) ? 0.2f : 1.0f);
        glPopMatrix();
    } else if (mode == TransformMode::Rotate) {
        axisColor(axis, TransformAxis::X, 1.0f, 0.1f, 0.1f);
        glPushMatrix();
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        drawRing(1.05f);
        glPopMatrix();
        axisColor(axis, TransformAxis::Y, 0.1f, 1.0f, 0.1f);
        glPushMatrix();
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        drawRing(1.05f);
        glPopMatrix();
        axisColor(axis, TransformAxis::Z, 0.1f, 0.35f, 1.0f);
        drawRing(1.05f);
    }

    glPopMatrix();
    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Renderer::drawCornerGizmo(const CameraState& camera, int width, int height) const {
    const int size = 130;
    glViewport(width - size - 10, height - size - 10, size, size);
    glClear(GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    const float yaw = radians(camera.yaw);
    const float pitch = radians(camera.pitch);
    const float x = std::cos(pitch) * std::sin(yaw);
    const float y = std::sin(pitch);
    const float z = std::cos(pitch) * std::cos(yaw);
    gluLookAt(x * 3.0f, y * 3.0f, z * 3.0f, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glDisable(GL_LIGHTING);
    glLineWidth(3.0f);
    drawAxis(1.0f, 1.0f, 0.1f, 0.1f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    drawAxis(1.0f, 0.1f, 1.0f, 0.1f);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
    drawAxis(1.0f, 0.1f, 0.35f, 1.0f);
    glPopMatrix();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}
