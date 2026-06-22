#include <GL/glut.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>

struct Vec3 {
    float x, y, z;
};

const GLfloat kLightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
const GLfloat kMaterialSpecular[4] = {0.95f, 0.95f, 0.95f, 1.0f};
const GLfloat kShininess = 48.0f;
float rotationY = -22.0f;

float dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 normalize(Vec3 v) {
    float length = std::sqrt(dot(v, v));
    return {v.x / length, v.y / length, v.z / length};
}

Vec3 reflect(Vec3 lightToSurface, Vec3 normal) {
    float scale = 2.0f * dot(normal, lightToSurface);
    return normalize({scale * normal.x - lightToSurface.x,
                      scale * normal.y - lightToSurface.y,
                      scale * normal.z - lightToSurface.z});
}

void printCalculation() {
    Vec3 normal = normalize({0.0f, 0.0f, 1.0f});
    Vec3 light = normalize({0.25f, 0.20f, 1.0f});
    Vec3 view = normalize({0.0f, 0.0f, 1.0f});
    Vec3 reflection = reflect(light, normal);
    float rdotv = std::max(dot(reflection, view), 0.0f);
    float factor = std::pow(rdotv, kShininess);

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Ejemplo: luz especular\n";
    std::cout << "Formula: I_specular = L_specular * K_specular * pow(max(dot(R,V),0), shininess)\n";
    std::cout << "N = (" << normal.x << ", " << normal.y << ", " << normal.z << ")\n";
    std::cout << "L = (" << light.x << ", " << light.y << ", " << light.z << ")\n";
    std::cout << "V = (" << view.x << ", " << view.y << ", " << view.z << ")\n";
    std::cout << "R = (" << reflection.x << ", " << reflection.y << ", " << reflection.z << ")\n";
    std::cout << "dot(R,V) = " << dot(reflection, view) << ", shininess = " << kShininess << "\n";
    std::cout << "factor especular = " << factor << "\n";
    std::cout << "I_specular = ("
              << kLightSpecular[0] * kMaterialSpecular[0] * factor << ", "
              << kLightSpecular[1] * kMaterialSpecular[1] * factor << ", "
              << kLightSpecular[2] * kMaterialSpecular[2] * factor << ")\n" << std::flush;
}

void drawText(float x, float y, const char* text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 4.2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    const GLfloat lightPosition[4] = {1.0f, 1.2f, 3.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glutSolidSphere(1.25, 96, 96);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(width) / height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    if (key == 27 || key == 'q' || key == 'Q') std::exit(0);
}

void special(int key, int, int) {
    if (key == GLUT_KEY_LEFT) rotationY -= 5.0f;
    if (key == GLUT_KEY_RIGHT) rotationY += 5.0f;
    glutPostRedisplay();
}

void initOpenGL() {
    glClearColor(0.04f, 0.04f, 0.055f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    const GLfloat ambient[4] = {0.04f, 0.04f, 0.05f, 1.0f};
    const GLfloat diffuse[4] = {0.18f, 0.30f, 0.70f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, kLightSpecular);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, kMaterialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, kShininess);
}

int main(int argc, char** argv) {
    printCalculation();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Luz especular - brillo reflejado");
    initOpenGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
