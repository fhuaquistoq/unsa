#include <cstdlib>
#include <cstdio>
#include <GL/glut.h>

#define RED 0.05f
#define GREEN 0.05f
#define BLUE 0.08f
#define ALPHA 1.0f
#define ECHAP 27

int windowWidth = 800;
int windowHeight = 800;

void init_scene();
void render_scene();

GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height);
GLvoid window_key(unsigned char key, int x, int y);

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 5: Orthographic Projection");

    initGL();
    init_scene();

    glutDisplayFunc(&window_display);
    glutReshapeFunc(&window_reshape);
    glutKeyboardFunc(&window_key);
    glutMainLoop();

    return 0;
}

GLvoid initGL()
{
    GLfloat lightPosition[] = {0.0f, 8.0f, 10.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(RED, GREEN, BLUE, ALPHA);
}

void drawObjects()
{
    glPushMatrix();
    glTranslatef(-6.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.75f, 0.15f);
    glutSolidCube(4.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -4.0f);
    glColor3f(0.0f, 0.8f, 1.0f);
    glutSolidSphere(2.2f, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.0f, 0.0f, -8.0f);
    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.2f, 0.9f);
    glutSolidTorus(0.7f, 2.0f, 32, 32);
    glPopMatrix();
}

GLvoid window_display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 6.0f, 24.0f,
              0.0f, 0.0f, -4.0f,
              0.0f, 1.0f, 0.0f);

    drawObjects();

    glutSwapBuffers();
}

GLvoid window_reshape(GLsizei width, GLsizei height)
{
    if (height == 0)
        height = 1;

    windowWidth = width;
    windowHeight = height;
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (aspect >= 1.0f)
        glOrtho(-14.0f * aspect, 14.0f * aspect, -14.0f, 14.0f, 1.0f, 80.0f);
    else
        glOrtho(-14.0f, 14.0f, -14.0f / aspect, 14.0f / aspect, 1.0f, 80.0f);

    glMatrixMode(GL_MODELVIEW);
}

void init_scene()
{
}

void render_scene()
{
    drawObjects();
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
