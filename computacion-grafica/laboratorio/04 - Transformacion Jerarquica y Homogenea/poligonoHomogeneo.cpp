#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;
float angle = 0.0f;
float positionX = 0.0f;
float positionY = 0.0f;
float scaleValue = 1.0f;

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

void translation(float m[16], float tx, float ty)
{
    loadIdentity(m);
    m[12] = tx;
    m[13] = ty;
}

void rotation(float m[16], float degrees)
{
    float radians = degrees * M_PI / 180.0f;
    loadIdentity(m);
    m[0] = cosf(radians);
    m[1] = sinf(radians);
    m[4] = -sinf(radians);
    m[5] = cosf(radians);
}

void scale(float m[16], float sx, float sy)
{
    loadIdentity(m);
    m[0] = sx;
    m[5] = sy;
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 4: Poligono homogeneo");

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
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glClearColor(RED, GREEN, BLUE, ALPHA);
    lastFrameTime = glutGet(GLUT_ELAPSED_TIME);
}

GLvoid window_display()
{
    float t[16], r[16], s[16];

    translation(t, positionX, positionY);
    rotation(r, angle);
    scale(s, scaleValue, scaleValue);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glMultMatrixf(t);
    glMultMatrixf(r);
    glMultMatrixf(s);

    glColor3f(1.0f, 0.7f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-4.0f, -3.0f, 0.0f);
    glVertex3f(4.0f, -3.0f, 0.0f);
    glVertex3f(5.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 4.0f, 0.0f);
    glVertex3f(-5.0f, 1.0f, 0.0f);
    glEnd();
    glPopMatrix();

    angle += 90.0f * deltaTime / 1000.0f;
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
    float step = 10.0f * deltaTime / 1000.0f;
    switch (key)
    {
    case ECHAP:
        exit(1);
        break;
    case 'a':
        positionX -= step;
        break;
    case 'd':
        positionX += step;
        break;
    case 'w':
        positionY += step;
        break;
    case 's':
        positionY -= step;
        break;
    case 'q':
        scaleValue -= 0.8f * deltaTime / 1000.0f;
        if (scaleValue < 0.2f)
            scaleValue = 0.2f;
        break;
    case 'e':
        scaleValue += 0.8f * deltaTime / 1000.0f;
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
