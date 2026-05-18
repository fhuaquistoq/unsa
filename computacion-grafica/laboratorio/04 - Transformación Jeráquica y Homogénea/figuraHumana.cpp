#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;
float walkTime = 0.0f;
float humanAngle = 15.0f;

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

void drawCube(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawLimb(float angle, float sx, float sy, float sz, float r, float g, float b)
{
    glRotatef(angle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, -sy * 0.5f, 0.0f);
    glColor3f(r, g, b);
    drawCube(sx, sy, sz);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 4: Figura humana");

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

    float swing = sinf(walkTime) * 35.0f;

    glRotatef(humanAngle, 0.0f, 1.0f, 0.0f);

    glPushMatrix();
    glTranslatef(0.0f, 3.0f, 0.0f);
    glColor3f(0.4f, 0.4f, 0.4f);
    drawCube(4.0f, 6.0f, 2.0f);

    glPushMatrix();
    glTranslatef(0.0f, 4.0f, 0.0f);
    glColor3f(0.4f, 0.4f, 0.4f);
    drawCube(3.2f, 3.2f, 3.2f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-2.8f, 2.8f, 0.0f);
    drawLimb(swing, 1.1f, 6.0f, 1.1f, 0.8f, 0.8f, 0.8f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.8f, 2.8f, 0.0f);
    drawLimb(-swing, 1.1f, 6.0f, 1.1f, 0.8f, 0.8f, 0.8f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.0f, -3.0f, 0.0f);
    drawLimb(-swing, 1.8f, 6.0f, 1.8f, 0.8f, 0.8f, 0.8f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0f, -3.0f, 0.0f);
    drawLimb(swing, 1.8f, 6.0f, 1.8f, 0.8f, 0.8f, 0.8f);
    glPopMatrix();
    glPopMatrix();

    walkTime += 4.0f * deltaTime / 1000.0f;

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
    case 'A':
        humanAngle -= step;
        break;
    case 'd':
    case 'D':
        humanAngle += step;
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
