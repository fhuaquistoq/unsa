#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

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

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 4: Ejemplo con Push Pop");

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

    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(-10.0f, 8.0f, 0.0f);
    glutSolidTeapot(3.0f);
    glPopMatrix();

    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glTranslatef(10.0f, 8.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);
    glutSolidTeapot(3.0f);
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
