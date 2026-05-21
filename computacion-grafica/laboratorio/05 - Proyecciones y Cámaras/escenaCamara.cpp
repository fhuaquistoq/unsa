#include <cmath>
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

float eyeX = 0.0f;
float eyeY = 4.0f;
float eyeZ = 18.0f;
float centerX = 0.0f;
float centerY = 0.0f;
float centerZ = 0.0f;

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
    glutCreateWindow("Lab 5: Keyboard Camera");

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

void zoomCamera(float amount)
{
    float dirX = centerX - eyeX;
    float dirY = centerY - eyeY;
    float dirZ = centerZ - eyeZ;
    float length = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);

    if (length <= 0.001f)
        return;

    dirX /= length;
    dirY /= length;
    dirZ /= length;

    eyeX += dirX * amount;
    eyeY += dirY * amount;
    eyeZ += dirZ * amount;
}

GLvoid window_display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ,
              centerX, centerY, centerZ,
              0.0f, 1.0f, 0.0f);

    glPushMatrix();
    glTranslatef(-5.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.75f, 0.15f);
    glutSolidCube(3.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -3.0f);
    glColor3f(0.0f, 0.8f, 1.0f);
    glutSolidSphere(1.8f, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(5.0f, 0.0f, -6.0f);
    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.2f, 0.9f);
    glutSolidTorus(0.6f, 1.6f, 32, 32);
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
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void init_scene()
{
}

GLvoid window_key(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    const float moveStep = 0.6f;
    const float zoomStep = 1.0f;

    switch (key)
    {
    case ECHAP:
        exit(0);
        break;
    case 'a':
        eyeX -= moveStep;
        centerX -= moveStep;
        break;
    case 'd':
        eyeX += moveStep;
        centerX += moveStep;
        break;
    case 'w':
        eyeY += moveStep;
        centerY += moveStep;
        break;
    case 's':
        eyeY -= moveStep;
        centerY -= moveStep;
        break;
    case '+':
    case 'q':
        zoomCamera(zoomStep);
        break;
    case '-':
    case 'e':
        zoomCamera(-zoomStep);
        break;
    default:
        printf("Inactive key: %d\n", key);
        break;
    }

    glutPostRedisplay();
}
