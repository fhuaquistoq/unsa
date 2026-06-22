#include <GL/glut.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>

const float kPi = 3.1415926535f;
float rotationY = -18.0f;

struct Vec3 {
    float x, y, z;
};

float dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 normalize(Vec3 v) {
    float length = std::sqrt(dot(v, v));
    return {v.x / length, v.y / length, v.z / length};
}

void printCalculation() {
    Vec3 light = normalize({0.35f, 0.25f, 1.0f});
    Vec3 correctNormal = normalize({0.0f, 0.0f, 1.0f});
    Vec3 badNormal = normalize({0.0f, 0.0f, -1.0f});

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Ejemplo: normales correctas vs normales invertidas\n";
    std::cout << "La luz difusa usa max(dot(N,L), 0). Si N apunta al lado equivocado, el producto punto cae.\n";
    std::cout << "L = (" << light.x << ", " << light.y << ", " << light.z << ")\n";
    std::cout << "Normal correcta = (" << correctNormal.x << ", " << correctNormal.y << ", " << correctNormal.z << ")\n";
    std::cout << "dot(N_correcta,L) = " << dot(correctNormal, light)
              << ", intensidad = " << std::max(dot(correctNormal, light), 0.0f) << "\n";
    std::cout << "Normal mala/invertida = (" << badNormal.x << ", " << badNormal.y << ", " << badNormal.z << ")\n";
    std::cout << "dot(N_mala,L) = " << dot(badNormal, light)
              << ", intensidad = " << std::max(dot(badNormal, light), 0.0f) << "\n" << std::flush;
}

void drawText(float x, float y, const char* text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 900, 0, 600);
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

void drawSphereWithNormals(bool invertNormals) {
    const int slices = 72;
    const int stacks = 36;
    const float radius = 1.0f;

    for (int i = 0; i < stacks; ++i) {
        float phi0 = -kPi / 2.0f + kPi * i / stacks;
        float phi1 = -kPi / 2.0f + kPi * (i + 1) / stacks;
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float theta = 2.0f * kPi * j / slices;
            Vec3 n0 = {std::cos(phi0) * std::cos(theta), std::sin(phi0), std::cos(phi0) * std::sin(theta)};
            Vec3 n1 = {std::cos(phi1) * std::cos(theta), std::sin(phi1), std::cos(phi1) * std::sin(theta)};
            Vec3 used0 = invertNormals ? Vec3{-n0.x, -n0.y, -n0.z} : n0;
            Vec3 used1 = invertNormals ? Vec3{-n1.x, -n1.y, -n1.z} : n1;
            glNormal3f(used0.x, used0.y, used0.z);
            glVertex3f(radius * n0.x, radius * n0.y, radius * n0.z);
            glNormal3f(used1.x, used1.y, used1.z);
            glVertex3f(radius * n1.x, radius * n1.y, radius * n1.z);
        }
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 6.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    const GLfloat lightPosition[4] = {1.8f, 1.4f, 4.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    const GLfloat goodDiffuse[4] = {0.20f, 0.55f, 1.0f, 1.0f};
    const GLfloat badDiffuse[4] = {1.0f, 0.22f, 0.16f, 1.0f};

    glPushMatrix();
    glTranslatef(-1.55f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, goodDiffuse);
    drawSphereWithNormals(false);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.55f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, badDiffuse);
    drawSphereWithNormals(true);
    glPopMatrix();

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
    glClearColor(0.04f, 0.045f, 0.055f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_CULL_FACE);

    const GLfloat ambientLight[4] = {0.10f, 0.10f, 0.10f, 1.0f};
    const GLfloat diffuseLight[4] = {1.0f, 0.95f, 0.85f, 1.0f};
    const GLfloat specular[4] = {0.15f, 0.15f, 0.15f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    const GLfloat materialAmbient[4] = {0.08f, 0.08f, 0.08f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16.0f);
}

int main(int argc, char** argv) {
    printCalculation();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Normales correctas vs normales malas");
    initOpenGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
