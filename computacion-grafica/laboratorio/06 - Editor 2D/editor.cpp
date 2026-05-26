#include <cstdlib>
#include <cstdio>
#include <GL/glut.h>
#include <vector>
#include <iostream>

#define RED 0.05f
#define GREEN 0.05f
#define BLUE 0.08f
#define ALPHA 1.0f
#define ECHAP 27

int windowWidth = 900;
int windowHeight = 800;

class Tool
{
public:
  virtual void draw() = 0;
  virtual void onMouseDown(int x, int y) = 0;
  virtual void onMouseUp(int x, int y) = 0;
  virtual void onMouseMove(int x, int y) = 0;
};

class PointTool : public Tool
{
private:
  int x, y;

public:
  PointTool()
  {
    this->x = -1;
    this->y = -1;
  }

  void draw() override
  {
    if (x < 100 || x > 1000 || y < 0 || y > 800)
      return;

    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(x, windowHeight - y);
    glEnd();
  }

  void onMouseDown(int x, int y) override
  {
    this->x = x;
    this->y = y;
  }

  void onMouseUp(int x, int y) override {}
  void onMouseMove(int x, int y) override {}
};

class LineTool : public Tool
{
private:
  int x1, y1, x2, y2;
  bool firstClick = false;

public:
  LineTool()
  {
    this->x1 = -1.0f;
    this->y1 = -1.0f;
    this->x2 = -1.0f;
    this->y2 = -1.0f;
  }

  void draw() override
  {
    if (x1 < 100 || x1 > 1000 || y1 < 0 || y1 > 800 || x2 < 100 || x2 > 1000 || y2 < 0 || y2 > 800)
      return;

    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(x1, windowHeight - y1);
    glVertex2i(x2, windowHeight - y2);
    glEnd();
  }

  void onMouseDown(int x, int y) override
  {
    if (!firstClick)
    {
      x1 = x;
      y1 = y;
      firstClick = true;
    }
    else
    {
      x2 = x;
      y2 = y;
      // firstClick = false;
    }
  }

  void onMouseUp(int x, int y) override {}
  void onMouseMove(int x, int y) override {}
};

class MultiLineTool : public Tool
{
private:
  std::vector<std::pair<int, int>> points;

public:
  void draw() override
  {
    if (points.size() < 2)
      return;
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0f, 0.0f, 1.0f);
    for (const auto &point : points)
    {
      int x = point.first;
      int y = point.second;
      if (x < 100 || x > 1000 || y < 0 || y > 800)
        continue;
      glVertex2i(x, windowHeight - y);
    }
    glEnd();
  }

  void onMouseDown(int x, int y) override
  {
    points.emplace_back(x, y);
  }

  void onMouseUp(int x, int y) override {}
  void onMouseMove(int x, int y) override {}
};

class PolygonTool : public Tool
{
private:
  std::vector<std::pair<int, int>> points;

public:
  void draw() override
  {
    if (points.size() < 3)
      return;
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 0.0f, 1.0f);
    for (const auto &point : points)
    {
      int x = point.first;
      int y = point.second;
      if (x < 100 || x > 1000 || y < 0 || y > 800)
        continue;
      glVertex2i(x, windowHeight - y);
    }
    std::pair<int, int> firstPoint = points.front();
    glVertex2i(firstPoint.first, windowHeight - firstPoint.second);
    glEnd();
  }

  void onMouseDown(int x, int y) override
  {
    points.emplace_back(x, y);
  }

  void onMouseUp(int x, int y) override {}
  void onMouseMove(int x, int y) override {}
};

std::vector<Tool *>
    tools;
Tool *currentTool = new PointTool();

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
  glutCreateWindow("Lab 6: Editor 2D");

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
  glClearColor(RED, GREEN, BLUE, ALPHA);
}

GLvoid window_display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  glBegin(GL_QUADS);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex3f(100.0f, 800.0f, 0.0f);
  glVertex3f(1000.0f, 800.0f, 0.0f);
  glVertex3f(1000.0f, 0.0f, 0.0f);
  glVertex3f(100.0f, 0.0f, 0.0f);
  glEnd();

  for (Tool *tool : tools)
    tool->draw();

  currentTool->draw();

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
  glOrtho(0.0f, (GLfloat)width, 0.0f, (GLfloat)height, -1.0f, 1.0f);
  glMatrixMode(GL_MODELVIEW);
}

void init_scene()
{
}

void render_scene()
{
}

GLvoid window_key(unsigned char key, int x, int y)
{
  switch (key)
  {
  case ECHAP:
    exit(0);
    break;
  case 'p':
  case 'P':
    tools.push_back(currentTool);
    currentTool = new PointTool();
    break;
  case 'l':
  case 'L':
    tools.push_back(currentTool);
    currentTool = new LineTool();
    break;
  case 'm':
  case 'M':
    tools.push_back(currentTool);
    currentTool = new MultiLineTool();
    break;
  case 'o':
  case 'O':
    tools.push_back(currentTool);
    currentTool = new PolygonTool();
    break;
  default:
    printf("Inactive key: %d\n", key);
    break;
  }
}

GLvoid window_mouse(int button, int state, int x, int y)
{
  if (x < 100 || x > 1000 || y < 0 || y > 800)
    return;

  std::cout << x << " " << y << std::endl;
  currentTool->onMouseDown(x, y);
  glutPostRedisplay();
}

GLvoid window_motion(int x, int y)
{
  glutPostRedisplay();
}
