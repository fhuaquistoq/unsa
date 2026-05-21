#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <GL/glut.h>

#define RED 0.02f
#define GREEN 0.02f
#define BLUE 0.05f
#define ALPHA 1.0f
#define ECHAP 27

const float PI = 3.1415926535f;

int windowWidth = 800;
int windowHeight = 800;
float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

float rotationSun = 0.0f;
float rotationEarth = 0.0f;
float translationEarth = 0.0f;
float rotationMoon = 0.0f;
float translationMoon = 0.0f;
float rotationMars = 0.0f;
float translationMars = 0.0f;

float rotationSunSpeed = 50.0f;
float rotationEarthSpeed = 180.0f;
float translationEarthSpeed = 35.0f;
float rotationMoonSpeed = 120.0f;
float translationMoonSpeed = 140.0f;
float rotationMarsSpeed = 90.0f;
float translationMarsSpeed = 22.0f;

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
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 5: Solar System Camera");

    initGL();
    init_scene();

    glutDisplayFunc(&window_display);
    glutReshapeFunc(&window_reshape);
    glutKeyboardFunc(&window_key);
    glutIdleFunc(&window_idle);
    glutMainLoop();

    return 0;
}

GLvoid initGL()
{
    GLfloat lightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(RED, GREEN, BLUE, ALPHA);

    lastFrameTime = (float)glutGet(GLUT_ELAPSED_TIME);
}

float degreesToRadians(float degrees)
{
    return degrees * PI / 180.0f;
}

void getOrbitPosition(float angleDegrees, float radius, float &x, float &z)
{
    float angle = degreesToRadians(angleDegrees);
    x = radius * cosf(angle);
    z = -radius * sinf(angle);
}

void wrapAngle(float &angle)
{
    if (angle >= 360.0f)
        angle -= 360.0f;
}

void updateAnimation()
{
    float seconds = deltaTime / 1000.0f;

    rotationSun += rotationSunSpeed * seconds;
    rotationEarth += rotationEarthSpeed * seconds;
    translationEarth += translationEarthSpeed * seconds;
    rotationMoon += rotationMoonSpeed * seconds;
    translationMoon += translationMoonSpeed * seconds;
    rotationMars += rotationMarsSpeed * seconds;
    translationMars += translationMarsSpeed * seconds;

    wrapAngle(rotationSun);
    wrapAngle(rotationEarth);
    wrapAngle(translationEarth);
    wrapAngle(rotationMoon);
    wrapAngle(translationMoon);
    wrapAngle(rotationMars);
    wrapAngle(translationMars);
}

GLvoid window_display()
{
    float earthX = 0.0f;
    float earthZ = 0.0f;

    getOrbitPosition(translationEarth, 10.0f, earthX, earthZ);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(earthX, 6.0f, earthZ + 14.0f,
              earthX, 0.0f, earthZ,
              0.0f, 1.0f, 0.0f);

    glPushMatrix();
    glRotatef(rotationSun, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 0.85f, 0.0f);
    glutSolidSphere(3.0f, 8, 8);
    glPopMatrix();

    glPushMatrix();
    glRotatef(translationEarth, 0.0f, 1.0f, 0.0f);
    glTranslatef(10.0f, 0.0f, 0.0f);

    glPushMatrix();
    glRotatef(rotationEarth, 0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 0.35f, 1.0f);
    glutSolidSphere(1.4f, 8, 8);
    glPopMatrix();

    glPushMatrix();
    glRotatef(translationMoon, 0.0f, 1.0f, 0.0f);
    glTranslatef(3.0f, 0.0f, 0.0f);
    glRotatef(rotationMoon, 0.0f, 1.0f, 0.0f);
    glColor3f(0.65f, 0.65f, 0.65f);
    glutSolidSphere(0.45f, 8, 8);
    glPopMatrix();

    glPopMatrix();

    glPushMatrix();
    glRotatef(translationMars, 0.0f, 1.0f, 0.0f);
    glTranslatef(18.0f, 0.0f, 0.0f);
    glRotatef(rotationMars, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 0.2f, 0.05f);
    glutSolidSphere(1.1f, 8, 8);
    glPopMatrix();

    glutSwapBuffers();
}

GLvoid window_reshape(GLsizei width, GLsizei height)
{
    if (height == 0)
        height = 1;

    windowWidth = width;
    windowHeight = height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 120.0f);
    glMatrixMode(GL_MODELVIEW);
}

void init_scene()
{
}

GLvoid window_key(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key)
    {
    case ECHAP:
        exit(0);
        break;
    default:
        printf("Inactive key: %d\n", key);
        break;
    }
}

GLvoid window_idle()
{
    float currentTime = (float)glutGet(GLUT_ELAPSED_TIME);
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    updateAnimation();
    glutPostRedisplay();
}
