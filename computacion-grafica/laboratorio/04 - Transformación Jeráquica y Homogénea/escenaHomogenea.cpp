#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;
float angle = 0.0f;
float travel = -12.0f;
float scaleValue = 0.6f;

#define RED 0
#define GREEN 0
#define BLUE 0
#define ALPHA 1
#define ECHAP 27

void init_scene();
void render_scene();

GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height);
GLvoid window_key(unsigned char key, int x, int y);
GLvoid window_idle();

void loadIdentity(float m[16])
{
    for (int i = 0; i < 16; i++)
        m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void translation(float m[16], float tx, float ty, float tz)
{
    loadIdentity(m);
    m[12] = tx;
    m[13] = ty;
    m[14] = tz;
}

void rotationY(float m[16], float degrees)
{
    float radians = degrees * M_PI / 180.0f;
    loadIdentity(m);
    m[0] = cosf(radians);
    m[2] = sinf(radians);
    m[8] = -sinf(radians);
    m[10] = cosf(radians);
}

void scale(float m[16], float sx, float sy, float sz)
{
    loadIdentity(m);
    m[0] = sx;
    m[5] = sy;
    m[10] = sz;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 4: Escena homogenea");

    initGL();
    init_scene();

    glutDisplayFunc(&window_display);
    glutReshapeFunc(&window_reshape);
    glutKeyboardFunc(&window_key);
    glutIdleFunc(&window_idle);
    glutMainLoop();

    return 1;
}

GLvoid initGL()
{
    GLfloat position[] = {0.0f, 5.0f, 10.0f, 0.0};

    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(RED, GREEN, BLUE, ALPHA);

    lastFrameTime = glutGet(GLUT_ELAPSED_TIME);
}

GLvoid window_display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float r[16], t[16], s[16];

    rotationY(r, angle);
    translation(t, travel, 0.0f, 0.0f);
    scale(s, scaleValue, scaleValue, scaleValue);

    glPushMatrix();
    glMultMatrixf(s);
    glMultMatrixf(t);
    glMultMatrixf(r);

    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidTeapot(3.0f);
    glPopMatrix();

    angle += 120.0f * deltaTime / 1000.0f;
    travel += 5.0f * deltaTime / 1000.0f;
    scaleValue += 0.25f * deltaTime / 1000.0f;

    if (travel > 12.0f)
    {
        travel = -12.0f;
        scaleValue = 0.6f;
    }

    if (angle >= 360.0f)
        angle -= 360.0f;

    glutSwapBuffers();
    glFlush();
}

GLvoid window_reshape(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-25.0f, 25.0f, -25.0f, 25.0f, -25.0f, 25.0f);
    glMatrixMode(GL_MODELVIEW);
}

void init_scene()
{
}

GLvoid window_key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case ECHAP:
        exit(1);
        break;
    default:
        printf("La touche %d non active.\n", key);
        break;
    }
}

GLvoid window_idle()
{
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    glutPostRedisplay();
}
