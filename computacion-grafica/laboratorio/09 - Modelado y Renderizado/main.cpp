#include <GL/glut.h>

#include <cstdlib>
#include <iostream>

#include "include/scene_common.hpp"
#include "include/texture_loader.hpp"

scene::CameraState g_camera;
scene::TextureBundle g_textures;
bool g_useDirectionalLight = true;

void configureLight()
{
  const GLfloat ambient[] = {0.20f, 0.20f, 0.20f, 1.0f};
  const GLfloat diffuse[] = {0.95f, 0.95f, 0.95f, 1.0f};
  const GLfloat specular[] = {1.00f, 1.00f, 1.00f, 1.0f};
  const GLfloat position[] = {10.0f, 14.0f, 10.0f, g_useDirectionalLight ? 0.0f : 1.0f};

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  if (g_useDirectionalLight)
  {
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
  }
  else
  {
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.03f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001f);
  }
}

void loadTextures()
{
  const texture_loader::LoadedTexture grass = texture_loader::loadNamedTexture("new_leaves.bmp");
  const texture_loader::LoadedTexture brick = texture_loader::loadNamedTexture("brick.bmp");
  const texture_loader::LoadedTexture roof = texture_loader::loadNamedTexture("roof_tiles.bmp");
  const texture_loader::LoadedTexture wood = texture_loader::loadNamedTexture("wood.bmp");
  const texture_loader::LoadedTexture leaves = texture_loader::loadNamedTexture("leaves.bmp");

  g_textures.terrain = grass.id;
  g_textures.walls = brick.id;
  g_textures.roof = roof.id;
  g_textures.trunk = wood.id;
  g_textures.canopy = leaves.id;
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  scene::applyCamera(g_camera);
  configureLight();

  scene::drawScene(scene::RenderMode::Textures, &g_textures);

  glutSwapBuffers();
}

void reshape(int width, int height)
{
  scene::applyProjection(width, height);
}

void special(int key, int, int)
{
  scene::adjustCameraOnArrowKey(g_camera, key);
  glutPostRedisplay();
}

void keyboard(unsigned char key, int, int)
{
  switch (key)
  {
  case '+':
    scene::zoomCamera(g_camera, -2.0f);
    break;
  case '-':
    scene::zoomCamera(g_camera, 2.0f);
    break;
  case 't':
  case 'T':
    g_useDirectionalLight = !g_useDirectionalLight;
    std::cout << "Modo de luz: "
              << (g_useDirectionalLight ? "direccional" : "puntual")
              << std::endl;
    break;
  case 27:
    std::exit(0);
  default:
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(1100, 780);
  glutCreateWindow("Lab 9 - 04 Texturizado");

  scene::initializeCommonState();
  scene::initializeRecommendedLightingState();
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  loadTextures();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
