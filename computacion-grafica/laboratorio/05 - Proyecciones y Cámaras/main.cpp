#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

using namespace std;

bool directionRight = true;
float rotationAngleTeapot = 0.0f;
float rotationAngleTorus = 0.0f;
float rotationAngleCube = 0.0f;
float translateX = 0.0f;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

float vs = 50.0f;
float vt = 100.0f;

float rotationSunSpeed = vs;
float rotationEarthSpeed = vt * 3.0f;
float translationEarthSpeed = vt;
float rotationMoonSpeed = vt * 1.5f;
float translationMoonSpeed = vt * 2.0f;
float rotationMarsSpeed = vs;
float translationMarsSpeed = vt * 0.8f;

float rotationSun = 0.0f;
float rotationEarth = 0.0f;
float translationEarth = 0.0f;
float rotationMoon = 0.0f;
float translationMoon = 0.0f;
float rotationMars = 0.0f;
float translationMars = 0.0f;

#define RED 0
#define GREEN 0
#define BLUE 0
#define ALPHA 1

#define ECHAP 27

void init_scene();
void render_scene();

GLvoid initGL();
GLvoid window_display();
GLvoid window_display_teapot();
GLvoid window_reshape(GLsizei width, GLsizei height);
GLvoid window_key(unsigned char key, int x, int y);

// function called on each frame
GLvoid window_idle();

int main(int argc, char **argv)
{

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 2: Transformaciones");

    initGL();
    init_scene();

    glutDisplayFunc(&window_display);

    glutReshapeFunc(&window_reshape);

    glutKeyboardFunc(&window_key);

    // function called on each frame
    glutIdleFunc(&window_idle);

    glutMainLoop();

    return 1;
}

GLvoid initGL()
{
    GLfloat position[] = {0.0f, 5.0f, 10.0f, 0.0};

    // enable light: try without it
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHTING);
    // light 0 "on": try without it
    glEnable(GL_LIGHT0);

    // shading model : try GL_FLAT
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);

    // enable material : try without it
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(RED, GREEN, BLUE, ALPHA);

    lastFrameTime = glutGet(GLUT_ELAPSED_TIME);
}

GLvoid window_display_teapot()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-25.0f, 25.0f, -25.0f, 25.0f, -25.0f, 25.0f);

    glRotatef(rotationAngleCube, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 8.0f, 0.0f);

    glColor3f(1.0f, 0.0f, 1.0f);
    glutSolidCube(2.0f);

    glRotatef(rotationAngleCube, -1.0f, 0.0f, 0.0f);
    glTranslated(0.0f, -8.0f, 0.0f);

    rotationAngleCube += 90.0f * (deltaTime / 1000.0f);

    glTranslatef(translateX, 0.0f, 0.0f);
    glRotatef(rotationAngleTeapot, 0.0f, 0.0f, 1.0f);
    glTranslatef(8.0f, 0.0f, 0.0f);

    rotationAngleTeapot += 90.0f * (deltaTime / 1000.0f);

    if (directionRight)
        translateX += 5.0f * (deltaTime / 1000.0f);
    else
        translateX -= 5.0f * (deltaTime / 1000.0f);

    if (translateX > 20.0f)
        directionRight = false;
    else if (translateX < -20.0f)
        directionRight = true;

    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidTeapot(2.0f);

    glRotatef(rotationAngleTorus, 0.0f, 1.0f, 1.0f);
    glTranslatef(6.0f, 0.0f, 0.0f);

    rotationAngleTorus += 270.0f * (deltaTime / 1000.0f);

    glColor3f(0.0f, 1.0f, 1.0f);
    glutSolidTorus(0.5f, 1.0f, 20, 20);

    if (rotationAngleCube > 360.0f)
        rotationAngleCube -= 360.0f;
    if (rotationAngleTeapot > 360.0f)
        rotationAngleTeapot -= 360.0f;
    if (rotationAngleTorus > 360.0f)
        rotationAngleTorus -= 360.0f;

    glutSwapBuffers();

    glFlush();
}

GLvoid window_display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-25.0f, 25.0f, -25.0f, 25.0f, -25.0f, 25.0f);

    // Sol
    glRotatef(rotationSun, 0.0f, 1.0f, 0.0f);

    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(4.0f, 8.0f, 8.0f);

    glRotatef(rotationSun, 0.0f, -1.0f, 0.0f);

    // Tierra
    glRotatef(translationEarth, 0.0f, 1.0f, 0.0f);
    glTranslatef(10.0f, 0.0f, 0.0f);
    glRotatef(rotationEarth, 0.0f, 1.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glutSolidSphere(2.0f, 8.0f, 8.0f);

    // Luna
    glRotatef(translationMoon, 0.0f, 1.0f, 0.0f);
    glTranslatef(3.0f, 0.0f, 0.0f);
    glRotatef(rotationMoon, 0.0f, 1.0f, 0.0f);

    glColor3f(0.5f, 0.5f, 0.5f);
    glutSolidSphere(0.5f, 8.0f, 8.0f);

    glRotatef(rotationMoon, 0.0f, -1.0f, 0.0f);
    glTranslatef(-3.0f, 0.0f, 0.0f);
    glRotatef(translationMoon, 0.0f, -1.0f, 0.0f);

    glRotatef(rotationEarth, 0.0f, -1.0f, 0.0f);
    glTranslatef(-10.0f, 0.0f, 0.0f);
    glRotatef(translationEarth, 0.0f, -1.0f, 0.0f);

    // Marte
    glRotatef(translationMars, 0.0f, 1.0f, 0.0f);
    glTranslatef(18.0f, 0.0f, 0.0f);
    glRotatef(rotationMars, 0.0f, 1.0f, 0.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glutSolidSphere(2.5f, 8.0f, 8.0f);

    rotationSun += rotationSunSpeed * (deltaTime / 1000.0f);
    rotationEarth += rotationEarthSpeed * (deltaTime / 1000.0f);
    translationEarth += translationEarthSpeed * (deltaTime / 1000.0f);
    rotationMoon += rotationMoonSpeed * (deltaTime / 1000.0f);
    translationMoon += translationMoonSpeed * (deltaTime / 1000.0f);
    rotationMars += rotationMarsSpeed * (deltaTime / 1000.0f);
    translationMars += translationMarsSpeed * (deltaTime / 1000.0f);

    if (rotationSun > 360.0f)
        rotationSun -= 360.0f;
    if (rotationEarth > 360.0f)
        rotationEarth -= 360.0f;
    if (translationEarth > 360.0f)
        translationEarth -= 360.0f;
    if (rotationMoon > 360.0f)
        rotationMoon -= 360.0f;
    if (translationMoon > 360.0f)
        translationMoon -= 360.0f;
    if (rotationMars > 360.0f)
        rotationMars -= 360.0f;
    if (translationMars > 360.0f)
        translationMars -= 360.0f;

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

// function called on each frame
GLvoid window_idle()
{
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    glutPostRedisplay();
}
