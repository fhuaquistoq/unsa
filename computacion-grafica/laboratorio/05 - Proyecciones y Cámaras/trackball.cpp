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
bool draggingLeft = false;
int lastMouseX = 0;
int lastMouseY = 0;
float trackballAngleX = 20.0f;
float trackballAngleY = -30.0f;
float cameraDistance = 22.0f;

void init_scene();
void render_scene();

GLvoid initGL();
GLvoid window_display();
GLvoid window_reshape(GLsizei width, GLsizei height);
GLvoid window_key(unsigned char key, int x, int y);
GLvoid window_mouse(int button, int state, int x, int y);
GLvoid window_motion(int x, int y);

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Lab 5: Trackball Camera");

    initGL();
    init_scene();

    glutDisplayFunc(&window_display);
    glutReshapeFunc(&window_reshape);
    glutKeyboardFunc(&window_key);
    glutMouseFunc(&window_mouse);
    glutMotionFunc(&window_motion);
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
}

void drawAxis(float r, float g, float b, float x, float y, float z)
{
    glColor3f(r, g, b);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(x, y, z);
    glEnd();
}

void drawGizmo()
{
    glDisable(GL_LIGHTING);
    glLineWidth(3.0f);
    drawAxis(1.0f, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f);
    drawAxis(0.0f, 1.0f, 0.0f, 0.0f, 5.0f, 0.0f);
    drawAxis(0.0f, 0.2f, 1.0f, 0.0f, 0.0f, 5.0f);
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

GLvoid window_display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, cameraDistance,
              0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);
    glRotatef(trackballAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef(trackballAngleY, 0.0f, 1.0f, 0.0f);

    drawGizmo();
    drawObjects();

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

GLvoid window_mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        draggingLeft = (state == GLUT_DOWN);
        lastMouseX = x;
        lastMouseY = y;
    }
    else if (state == GLUT_DOWN && button == 3)
    {
        cameraDistance -= 1.0f;
        if (cameraDistance < 6.0f)
            cameraDistance = 6.0f;
    }
    else if (state == GLUT_DOWN && button == 4)
    {
        cameraDistance += 1.0f;
        if (cameraDistance > 60.0f)
            cameraDistance = 60.0f;
    }

    glutPostRedisplay();
}

GLvoid window_motion(int x, int y)
{
    if (!draggingLeft)
        return;

    int dx = x - lastMouseX;
    int dy = y - lastMouseY;

    trackballAngleY += dx * 0.5f;
    trackballAngleX += dy * 0.5f;
    lastMouseX = x;
    lastMouseY = y;

    glutPostRedisplay();
}
