#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;
float angle = 0.0f;
float positionX = -8.0f;
float direction = 1.0f;
float cubeAngle = 0.0f;

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

void rotationX(float m[16], float degrees)
{
    const float pi = 3.14159265f;
    float radians = degrees * pi / 180.0f;
    loadIdentity(m);
    m[5] = cosf(radians);
    m[6] = sinf(radians);
    m[9] = -sinf(radians);
    m[10] = cosf(radians);
}

void rotationY(float m[16], float degrees)
{
    const float pi = 3.14159265f;
    float radians = degrees * pi / 180.0f;
    loadIdentity(m);
    m[0] = cosf(radians);
    m[2] = -sinf(radians);
    m[8] = sinf(radians);
    m[10] = cosf(radians);
}

void rotationZ(float m[16], float degrees)
{
    const float pi = 3.14159265f;
    float radians = degrees * pi / 180.0f;
    loadIdentity(m);
    m[0] = cosf(radians);
    m[1] = sinf(radians);
    m[4] = -sinf(radians);
    m[5] = cosf(radians);
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
    glutCreateWindow("Lab 4: Escena Lab 3 homogenea");

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
    GLfloat position[] = {0.0f, 8.0f, 12.0f, 0.0};

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

    float teapotCenter[16], teapotOrbit[16], teapotOffset[16];
    float torusOrbit[16], torusOffset[16];
    float cubeCenter[16], cubeOrbit[16], cubeOffset[16], cubeS[16];

    translation(teapotCenter, positionX, 0.0f, 0.0f);
    rotationZ(teapotOrbit, angle);
    translation(teapotOffset, 4.0f, 0.0f, 0.0f);

    rotationZ(torusOrbit, angle * 3.0f);
    translation(torusOffset, 3.0f, 0.0f, 0.0f);

    translation(cubeCenter, 0.0f, 0.0f, 0.0f);
    rotationX(cubeOrbit, cubeAngle);
    translation(cubeOffset, 0.0f, 4.0f, 0.0f);
    scale(cubeS, 2.0f, 2.0f, 2.0f);

    glPushMatrix();
    glMultMatrixf(teapotCenter);
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.25f, 12, 12);

    glPushMatrix();
    glMultMatrixf(teapotOrbit);
    glMultMatrixf(teapotOffset);
    glColor3f(0.8f, 0.8f, 0.8f);
    glutSolidTeapot(1.5f);

    glPushMatrix();
    glMultMatrixf(torusOrbit);
    glMultMatrixf(torusOffset);
    glColor3f(0.7f, 0.7f, 0.7f);
    glutSolidTorus(0.25f, 0.7f, 16, 32);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glMultMatrixf(cubeCenter);
    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(0.25f, 12, 12);

    glPushMatrix();
    glMultMatrixf(cubeOrbit);
    glMultMatrixf(cubeOffset);
    glMultMatrixf(cubeS);
    glColor3f(0.5f, 0.5f, 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    positionX += direction * 5.0f * deltaTime / 1000.0f;
    angle += 100.0f * deltaTime / 1000.0f;
    cubeAngle += 130.0f * deltaTime / 1000.0f;

    if (positionX >= 8.0f)
    {
        positionX = 8.0f;
        direction = -1.0f;
    }
    else if (positionX <= -8.0f)
    {
        positionX = -8.0f;
        direction = 1.0f;
    }

    if (angle >= 360.0f)
        angle -= 360.0f;
    if (cubeAngle >= 360.0f)
        cubeAngle -= 360.0f;

    glutSwapBuffers();
    glFlush();
}

GLvoid window_reshape(GLsizei width, GLsizei height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-14.0f, 14.0f, -14.0f, 14.0f, -25.0f, 25.0f);
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
