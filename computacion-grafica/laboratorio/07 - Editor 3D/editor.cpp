#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

#define ESC 27

struct Point
{
  float x;
  float y;
  float z;
};

struct Color
{
  float r;
  float g;
  float b;
};

enum ObjectType
{
  CUBE,
  SPHERE,
  TORUS,
  TEAPOT
};

struct Object
{
  ObjectType type;
  Point position;
  Color color;
};

struct Camera
{
  Point position;
  Point target;
  Point up;
};

struct Button
{
  int x;
  int y;
  int width;
  int height;
  ObjectType type;
  void (*action)(void);
};

int windowWidth = 900;
int windowHeight = 800;

float screenY(float y)
{
  return windowHeight - y;
}

bool isInsideCanvas(int x, int y)
{
  return x >= 0 && x <= windowWidth && y >= 0 && y <= windowHeight;
}

void initializeOpenGL()
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  glutSwapBuffers();
}

void reshape(GLsizei width, GLsizei height)
{
  if (height == 0)
    height = 1;

  windowWidth = width;
  windowHeight = height;

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
  (void)x;
  (void)y;

  switch (key)
  {
  case ESC:
    exit(0);
    break;
  default:
    break;
  }

  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
  if (!isInsideCanvas(x, y))
    return;

  glutPostRedisplay();
}

void motion(int x, int y)
{
  if (!isInsideCanvas(x, y))
    return;

  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Lab 7: Editor 3D");

  initializeOpenGL();

  glutDisplayFunc(&display);
  glutReshapeFunc(&reshape);
  glutKeyboardFunc(&keyboard);
  glutMouseFunc(&mouse);
  glutMotionFunc(&motion);
  glutPassiveMotionFunc(&motion);
  glutMainLoop();

  return 0;
}