#include <GL/glut.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>

struct Vec3 {
    float x, y, z;
};

const GLfloat kLightDiffuse[4] = {1.0f, 0.95f, 0.85f, 1.0f};
const GLfloat kMaterialDiffuse[4] = {0.25f, 0.55f, 1.0f, 1.0f};
float rotationY = -18.0f;

float dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 normalize(Vec3 v) {
    float length = std::sqrt(dot(v, v));
    return {v.x / length, v.y / length, v.z / length};
}

void printCalculation() {
    Vec3 normal = normalize({0.0f, 0.0f, 1.0f});
    Vec3 light = normalize({0.45f, 0.35f, 1.0f});
    float ndotl = std::max(dot(normal, light), 0.0f);

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Ejemplo: luz difusa Lambert\n";
    std::cout << "Formula: I_diffuse = L_diffuse * K_diffuse * max(dot(N,L), 0)\n";
    std::cout << "N = (" << normal.x << ", " << normal.y << ", " << normal.z << ")\n";
    std::cout << "L = (" << light.x << ", " << light.y << ", " << light.z << ")\n";
    std::cout << "dot(N,L) = " << dot(normal, light) << ", max = " << ndotl << "\n";
    std::cout << "L_diffuse = (" << kLightDiffuse[0] << ", " << kLightDiffuse[1] << ", " << kLightDiffuse[2] << ")\n";
    std::cout << "K_diffuse = (" << kMaterialDiffuse[0] << ", " << kMaterialDiffuse[1] << ", " << kMaterialDiffuse[2] << ")\n";
    std::cout << "I_diffuse = ("
              << kLightDiffuse[0] * kMaterialDiffuse[0] * ndotl << ", "
              << kLightDiffuse[1] * kMaterialDiffuse[1] * ndotl << ", "
              << kLightDiffuse[2] * kMaterialDiffuse[2] * ndotl << ")\n" << std::flush;
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

    const GLfloat lightPosition[4] = {2.2f, 1.8f, 4.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glutSolidSphere(1.25, 64, 64);

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
    glClearColor(0.05f, 0.06f, 0.08f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    const GLfloat ambient[4] = {0.03f, 0.03f, 0.03f, 1.0f};
    const GLfloat zero[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, kLightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, zero);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, kMaterialDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
}

int main(int argc, char** argv) {
    printCalculation();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Luz difusa - reflexion Lambert");
    initOpenGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
