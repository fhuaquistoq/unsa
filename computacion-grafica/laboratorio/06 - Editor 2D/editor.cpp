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
};

struct Color
{
  float r;
  float g;
  float b;
};

enum ShapeType
{
  DOT,
  LINE,
  POLYLINE,
  POLYGON
};

struct Shape
{
  ShapeType type;
  std::vector<Point> vertices;
  Color stroke;
  Color fill;
  bool filled;
};

const Color WHITE = {1.0f, 1.0f, 1.0f};
const Color BLACK = {0.0f, 0.0f, 0.0f};
const Color RED = {1.0f, 0.0f, 0.0f};
const Color GREEN = {0.0f, 1.0f, 0.0f};
const Color BLUE = {0.0f, 0.0f, 1.0f};
const Color YELLOW = {1.0f, 1.0f, 0.0f};
const Color CYAN = {0.0f, 1.0f, 1.0f};
const Color MAGENTA = {1.0f, 0.0f, 1.0f};

int windowWidth = 900;
int windowHeight = 800;

std::vector<Color> palette = {WHITE, BLACK, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA};
std::vector<Shape> scene;
std::vector<int> selection;

Color strokeColor = BLACK;
Color fillColor = WHITE;
bool fillEnabled = false;
char transformMode = 0;
char pendingColorMode = 0;

float screenY(float y)
{
  return windowHeight - y;
}

bool isInsideCanvas(int x, int y)
{
  return x >= 0 && x <= windowWidth && y >= 0 && y <= windowHeight;
}

void useColor(const Color &color)
{
  glColor3f(color.r, color.g, color.b);
}

void drawPoints(const std::vector<Point> &points)
{
  glPointSize(6.0f);
  glBegin(GL_POINTS);
  useColor(RED);
  for (const Point &point : points)
    glVertex2f(point.x, screenY(point.y));
  glEnd();
}

void drawRectangle(float minX, float minY, float maxX, float maxY)
{
  glLineWidth(2.0f);
  glBegin(GL_LINE_LOOP);
  useColor(BLACK);
  glVertex2f(minX, screenY(minY));
  glVertex2f(maxX, screenY(minY));
  glVertex2f(maxX, screenY(maxY));
  glVertex2f(minX, screenY(maxY));
  glEnd();

  glPointSize(7.0f);
  glBegin(GL_POINTS);
  useColor(RED);
  glVertex2f(minX, screenY(minY));
  glVertex2f(maxX, screenY(minY));
  glVertex2f(maxX, screenY(maxY));
  glVertex2f(minX, screenY(maxY));
  glEnd();
}

void drawRange(Point start, Point end)
{
  drawRectangle(std::min(start.x, end.x), std::min(start.y, end.y), std::max(start.x, end.x), std::max(start.y, end.y));
}

void drawShape(const Shape &shape)
{
  if (shape.vertices.empty())
    return;

  if (shape.type == POLYGON && shape.filled && shape.vertices.size() >= 3)
  {
    glBegin(GL_POLYGON);
    useColor(shape.fill);
    for (const Point &point : shape.vertices)
      glVertex2f(point.x, screenY(point.y));
    glEnd();
  }

  glLineWidth(2.0f);
  useColor(shape.stroke);

  if (shape.type == DOT)
  {
    drawPoints(shape.vertices);
    return;
  }

  if (shape.type == LINE && shape.vertices.size() >= 2)
  {
    glBegin(GL_LINES);
    glVertex2f(shape.vertices[0].x, screenY(shape.vertices[0].y));
    glVertex2f(shape.vertices[1].x, screenY(shape.vertices[1].y));
    glEnd();
  }
  else if (shape.type == POLYLINE && shape.vertices.size() >= 2)
  {
    glBegin(GL_LINE_STRIP);
    for (const Point &point : shape.vertices)
      glVertex2f(point.x, screenY(point.y));
    glEnd();
  }
  else if (shape.type == POLYGON && shape.vertices.size() >= 2)
  {
    glBegin(GL_LINE_LOOP);
    for (const Point &point : shape.vertices)
      glVertex2f(point.x, screenY(point.y));
    glEnd();
  }

  drawPoints(shape.vertices);
}

bool isShapeInsideRange(const Shape &shape, Point start, Point end)
{
  float minX = std::min(start.x, end.x);
  float maxX = std::max(start.x, end.x);
  float minY = std::min(start.y, end.y);
  float maxY = std::max(start.y, end.y);

  for (const Point &point : shape.vertices)
    if (point.x < minX || point.x > maxX || point.y < minY || point.y > maxY)
      return false;

  return true;
}

void selectShapesInside(Point start, Point end)
{
  selection.clear();

  for (int i = 0; i < (int)scene.size(); ++i)
    if (isShapeInsideRange(scene[i], start, end))
      selection.push_back(i);
}

bool getSelectionBounds(float &minX, float &minY, float &maxX, float &maxY)
{
  bool found = false;

  for (int index : selection)
  {
    if (index < 0 || index >= (int)scene.size())
      continue;

    for (const Point &point : scene[index].vertices)
    {
      if (!found)
      {
        minX = maxX = point.x;
        minY = maxY = point.y;
        found = true;
      }
      else
      {
        minX = std::min(minX, point.x);
        maxX = std::max(maxX, point.x);
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);
      }
    }
  }

  return found;
}

void drawSelectionBounds()
{
  float minX, minY, maxX, maxY;

  if (getSelectionBounds(minX, minY, maxX, maxY))
    drawRectangle(minX - 10.0f, minY - 10.0f, maxX + 10.0f, maxY + 10.0f);
}

bool hasSelection()
{
  return !selection.empty();
}

Point getSelectionCenter(bool includeDots)
{
  Point center = {0.0f, 0.0f};
  int count = 0;

  for (int index : selection)
  {
    if (index < 0 || index >= (int)scene.size())
      continue;
    if (!includeDots && scene[index].type == DOT)
      continue;

    for (const Point &point : scene[index].vertices)
    {
      center.x += point.x;
      center.y += point.y;
      count++;
    }
  }

  if (count > 0)
  {
    center.x /= count;
    center.y /= count;
  }

  return center;
}

void moveSelection(float dx, float dy)
{
  for (int index : selection)
  {
    if (index < 0 || index >= (int)scene.size())
      continue;

    for (Point &point : scene[index].vertices)
    {
      point.x += dx;
      point.y += dy;
    }
  }
}

void rotateSelection(float degrees)
{
  Point center = getSelectionCenter(false);
  float radians = degrees * 3.14159265f / 180.0f;
  float cosValue = std::cos(radians);
  float sinValue = std::sin(radians);

  for (int index : selection)
  {
    if (index < 0 || index >= (int)scene.size() || scene[index].type == DOT)
      continue;

    for (Point &point : scene[index].vertices)
    {
      float x = point.x - center.x;
      float y = point.y - center.y;
      point.x = center.x + x * cosValue - y * sinValue;
      point.y = center.y + x * sinValue + y * cosValue;
    }
  }
}

void scaleSelection(float factor)
{
  Point center = getSelectionCenter(false);

  for (int index : selection)
  {
    if (index < 0 || index >= (int)scene.size() || scene[index].type == DOT)
      continue;

    for (Point &point : scene[index].vertices)
    {
      point.x = center.x + (point.x - center.x) * factor;
      point.y = center.y + (point.y - center.y) * factor;
    }
  }
}

void setSelectionStroke(Color color)
{
  for (int index : selection)
    if (index >= 0 && index < (int)scene.size())
      scene[index].stroke = color;
}

void setSelectionFill(Color color)
{
  for (int index : selection)
    if (index >= 0 && index < (int)scene.size())
      scene[index].fill = color;
}

void toggleSelectionFill()
{
  for (int index : selection)
    if (index >= 0 && index < (int)scene.size() && scene[index].type == POLYGON)
      scene[index].filled = !scene[index].filled;
}

class Tool
{
protected:
  Color stroke;
  Color fill;
  bool filled;
  bool ready = false;
  Shape result;

  Shape createShape(ShapeType type, const std::vector<Point> &vertices)
  {
    Shape shape;
    shape.type = type;
    shape.vertices = vertices;
    shape.stroke = stroke;
    shape.fill = fill;
    shape.filled = filled;
    return shape;
  }

public:
  Tool(Color strokeColor, Color fillColor, bool fillStatus)
  {
    stroke = strokeColor;
    fill = fillColor;
    filled = fillStatus;
  }

  virtual ~Tool() {}
  virtual char key() = 0;
  virtual void draw() = 0;
  virtual void onMouseDown(int x, int y) = 0;
  virtual void onMouseMove(int x, int y) = 0;
  virtual void finish() {}

  bool hasShapeReady()
  {
    return ready;
  }

  Shape takeShape()
  {
    ready = false;
    return result;
  }

  void updateStyle(Color strokeColor, Color fillColor, bool fillStatus)
  {
    stroke = strokeColor;
    fill = fillColor;
    filled = fillStatus;
  }
};

class DotTool : public Tool
{
public:
  DotTool(Color strokeColor, Color fillColor, bool fillStatus) : Tool(strokeColor, fillColor, fillStatus) {}

  char key() override
  {
    return 'd';
  }

  void draw() override {}

  void onMouseDown(int x, int y) override
  {
    result = createShape(DOT, {{(float)x, (float)y}});
    ready = true;
  }

  void onMouseMove(int x, int y) override
  {
    (void)x;
    (void)y;
  }
};

class LineTool : public Tool
{
private:
  bool hasFirstPoint = false;
  Point firstPoint;
  Point previewPoint;

public:
  LineTool(Color strokeColor, Color fillColor, bool fillStatus) : Tool(strokeColor, fillColor, fillStatus) {}

  char key() override
  {
    return 'l';
  }

  void draw() override
  {
    if (hasFirstPoint)
      drawShape(createShape(LINE, {firstPoint, previewPoint}));
  }

  void onMouseDown(int x, int y) override
  {
    Point point = {(float)x, (float)y};

    if (!hasFirstPoint)
    {
      firstPoint = point;
      previewPoint = point;
      hasFirstPoint = true;
      return;
    }

    result = createShape(LINE, {firstPoint, point});
    ready = true;
    hasFirstPoint = false;
  }

  void onMouseMove(int x, int y) override
  {
    if (hasFirstPoint)
      previewPoint = {(float)x, (float)y};
  }
};

class MultiLineTool : public Tool
{
private:
  std::vector<Point> points;
  Point previewPoint;
  bool hasPreview = false;

public:
  MultiLineTool(Color strokeColor, Color fillColor, bool fillStatus) : Tool(strokeColor, fillColor, fillStatus) {}

  char key() override
  {
    return 'm';
  }

  void draw() override
  {
    if (points.empty())
      return;

    std::vector<Point> preview = points;
    if (hasPreview)
      preview.push_back(previewPoint);
    drawShape(createShape(POLYLINE, preview));
  }

  void onMouseDown(int x, int y) override
  {
    Point point = {(float)x, (float)y};
    points.push_back(point);
    previewPoint = point;
    hasPreview = true;
  }

  void onMouseMove(int x, int y) override
  {
    if (hasPreview)
      previewPoint = {(float)x, (float)y};
  }

  void finish() override
  {
    if (points.size() < 2)
      return;

    result = createShape(POLYLINE, points);
    ready = true;
    points.clear();
    hasPreview = false;
  }
};

class PolygonTool : public Tool
{
private:
  std::vector<Point> points;
  Point previewPoint;
  bool hasPreview = false;

public:
  PolygonTool(Color strokeColor, Color fillColor, bool fillStatus) : Tool(strokeColor, fillColor, fillStatus) {}

  char key() override
  {
    return 'p';
  }

  void draw() override
  {
    if (points.empty())
      return;

    std::vector<Point> preview = points;
    if (hasPreview)
      preview.push_back(previewPoint);
    drawShape(createShape(POLYGON, preview));
  }

  void onMouseDown(int x, int y) override
  {
    Point point = {(float)x, (float)y};
    points.push_back(point);
    previewPoint = point;
    hasPreview = true;
  }

  void onMouseMove(int x, int y) override
  {
    if (hasPreview)
      previewPoint = {(float)x, (float)y};
  }

  void finish() override
  {
    if (points.size() < 3)
      return;

    result = createShape(POLYGON, points);
    ready = true;
    points.clear();
    hasPreview = false;
  }
};

class SelectionTool : public Tool
{
private:
  bool selecting = false;
  Point startPoint;
  Point previewPoint;

public:
  SelectionTool() : Tool(BLACK, WHITE, false) {}

  char key() override
  {
    return 's';
  }

  void draw() override
  {
    if (selecting)
      drawRange(startPoint, previewPoint);
  }

  void onMouseDown(int x, int y) override
  {
    Point point = {(float)x, (float)y};

    if (!selecting)
    {
      startPoint = point;
      previewPoint = point;
      selecting = true;
      selection.clear();
      return;
    }

    previewPoint = point;
    selectShapesInside(startPoint, previewPoint);
    selecting = false;
  }

  void onMouseMove(int x, int y) override
  {
    if (selecting)
      previewPoint = {(float)x, (float)y};
  }

  void finish() override
  {
    selecting = false;
    selection.clear();
  }
};

Tool *currentTool = new DotTool(strokeColor, fillColor, fillEnabled);

Tool *createTool(char key)
{
  if (key == 'l')
    return new LineTool(strokeColor, fillColor, fillEnabled);
  if (key == 'm')
    return new MultiLineTool(strokeColor, fillColor, fillEnabled);
  if (key == 'p')
    return new PolygonTool(strokeColor, fillColor, fillEnabled);
  if (key == 's')
    return new SelectionTool();
  return new DotTool(strokeColor, fillColor, fillEnabled);
}

void updateCurrentToolStyle()
{
  currentTool->updateStyle(strokeColor, fillColor, fillEnabled);
}

void switchTool(char key)
{
  delete currentTool;
  selection.clear();
  currentTool = createTool(key);
}

void storeFinishedShape()
{
  if (!currentTool->hasShapeReady())
    return;

  char key = currentTool->key();
  scene.push_back(currentTool->takeShape());
  delete currentTool;
  currentTool = createTool(key);
}

void applyPaletteColor(char colorKey)
{
  if (!pendingColorMode || colorKey < '1' || colorKey > '8')
    return;

  Color color = palette[colorKey - '1'];

  if (pendingColorMode == 'c')
  {
    strokeColor = color;
    setSelectionStroke(color);
  }

  if (pendingColorMode == 'b')
  {
    fillColor = color;
    setSelectionFill(color);
  }

  pendingColorMode = 0;
  updateCurrentToolStyle();
}

void toggleFillMode()
{
  if (hasSelection())
    toggleSelectionFill();
  else
  {
    fillEnabled = !fillEnabled;
    updateCurrentToolStyle();
  }
}

void runTransform(char key)
{
  if (key == '8')
  {
    if (transformMode == 't')
      moveSelection(0.0f, -10.0f);
    if (transformMode == 'k')
      scaleSelection(1.1f);
  }

  if (key == '2')
  {
    if (transformMode == 't')
      moveSelection(0.0f, 10.0f);
    if (transformMode == 'k')
      scaleSelection(0.9f);
  }

  if (key == '4')
  {
    if (transformMode == 't')
      moveSelection(-10.0f, 0.0f);
    if (transformMode == 'r')
      rotateSelection(-10.0f);
  }

  if (key == '6')
  {
    if (transformMode == 't')
      moveSelection(10.0f, 0.0f);
    if (transformMode == 'r')
      rotateSelection(10.0f);
  }
}

void initializeOpenGL()
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  for (const Shape &shape : scene)
    drawShape(shape);

  drawSelectionBounds();
  currentTool->draw();
  storeFinishedShape();

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

  if (pendingColorMode && key >= '1' && key <= '8')
  {
    applyPaletteColor(key);
    glutPostRedisplay();
    return;
  }

  pendingColorMode = 0;

  switch (key)
  {
  case ESC:
    exit(0);
    break;
  case 'd':
  case 'D':
    switchTool('d');
    break;
  case 'l':
  case 'L':
    switchTool('l');
    break;
  case 'm':
  case 'M':
    switchTool('m');
    break;
  case 'p':
  case 'P':
    switchTool('p');
    break;
  case 's':
  case 'S':
    switchTool('s');
    break;
  case 't':
  case 'T':
    transformMode = 't';
    break;
  case 'r':
  case 'R':
    transformMode = 'r';
    break;
  case 'k':
  case 'K':
    transformMode = 'k';
    break;
  case 'f':
  case 'F':
    toggleFillMode();
    break;
  case 'c':
  case 'C':
    pendingColorMode = 'c';
    break;
  case 'b':
  case 'B':
    pendingColorMode = 'b';
    break;
  case 13:
    currentTool->finish();
    storeFinishedShape();
    break;
  case '8':
  case '2':
  case '4':
  case '6':
    runTransform(key);
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

  if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
    currentTool->onMouseDown(x, y);

  storeFinishedShape();
  glutPostRedisplay();
}

void motion(int x, int y)
{
  if (!isInsideCanvas(x, y))
    return;

  currentTool->onMouseMove(x, y);
  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(windowWidth, windowHeight);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Lab 6: Editor 2D");

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
