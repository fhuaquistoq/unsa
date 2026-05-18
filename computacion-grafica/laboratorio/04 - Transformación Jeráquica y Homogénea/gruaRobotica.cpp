#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;
float baseAngle = 0.0f;
float upperArmAngle = -20.0f;
float forearmAngle = 35.0f;
float cableLength = 3.0f;

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

void drawCube(float sx, float sy, float sz, float r, float g, float b)
{
    glPushMatrix();
    glColor3f(r, g, b);
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 4: Grua robotica");

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

    glPushMatrix();
    glRotatef(baseAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, -8.0f, 0.0f);
    drawCube(4.0f, 1.0f, 4.0f, 1.0f, 1.0f, 0.0f);

    glRotatef(upperArmAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, 5.0f, 0.0f);
    drawCube(0.5f, 10.0f, 0.5f, 0.0f, 0.8f, 1.0f);

    glTranslatef(0.0f, 5.0f, 0.0f);
    glRotatef(forearmAngle, 0.0f, 0.0f, 1.0f);
    glTranslatef(3.0f, 0.0f, 0.0f);
    drawCube(6.0f, 0.45f, 0.45f, 1.0f, 0.0f, 1.0f);

    glTranslatef(3.0f, 0.0f, 0.0f);
    glRotatef(upperArmAngle + forearmAngle, 0.0f, 0.0f, -1.0f);
    glTranslatef(0.0f, -cableLength * 0.5f, 0.0f);
    drawCube(0.12f, cableLength, 0.12f, 0.0f, 1.0f, 0.0f);

    glTranslatef(0.0f, -cableLength * 0.5f - 0.7f, 0.0f);
    drawCube(1.4f, 1.4f, 1.4f, 1.0f, 0.0f, 0.0f);
    glPopMatrix();

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
    float step = 200.0f * deltaTime / 1000.0f;

    switch (key)
    {
    case ECHAP:
        exit(1);
        break;
    case 'a':
        baseAngle -= step;
        break;
    case 'd':
        baseAngle += step;
        break;
    case 'w':
        upperArmAngle -= step;
        break;
    case 's':
        upperArmAngle += step;
        break;
    case 'q':
        forearmAngle += step;
        break;
    case 'e':
        forearmAngle -= step;
        break;
    case 'z':
        cableLength += step * 0.08f;
        if (cableLength > 9.0f)
            cableLength = 9.0f;
        break;
    case 'x':
        cableLength -= step * 0.08f;
        if (cableLength < 1.0f)
            cableLength = 1.0f;
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
