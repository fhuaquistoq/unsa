#include <GL/glut.h>

#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>

GLuint textureId = 0;
float rotationY = -25.0f;

void printMappingInfo() {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Diagrama UV: Imagen -> UV -> Superficie 3D\n";
    std::cout << "Textura procedural checkerboard: no usa archivos externos.\n";
    std::cout << "Coordenadas ejemplo:\n";
    std::cout << "  vertice inferior izquierdo: posicion (-1,-1,0) -> UV (0,0)\n";
    std::cout << "  vertice inferior derecho:   posicion ( 1,-1,0) -> UV (1,0)\n";
    std::cout << "  vertice superior derecho:   posicion ( 1, 1,0) -> UV (1,1)\n";
    std::cout << "  vertice superior izquierdo: posicion (-1, 1,0) -> UV (0,1)\n";
    std::cout << "Pipeline: Imagen 2D -> coordenadas U/V interpoladas -> muestreo -> superficie 3D texturizada\n" << std::flush;
}

void drawText(float x, float y, const char* text) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    glEnable(GL_TEXTURE_2D);
}

void makeTexture() {
    const int size = 128;
    unsigned char data[size][size][3];
    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            bool dark = ((x / 16) + (y / 16)) % 2 == 0;
            data[y][x][0] = dark ? 30 : 235;
            data[y][x][1] = dark ? 70 : 235;
            data[y][x][2] = dark ? 130 : 80;
        }
    }

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void drawArrow(float x0, float y0, float x1, float y1) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 0.85f, 0.20f);
    glBegin(GL_LINES);
    glVertex2f(x0, y0);
    glVertex2f(x1, y1);
    glEnd();

    float dx = x1 - x0;
    float dy = y1 - y0;
    float horizontal = dx >= 0.0f ? 1.0f : -1.0f;
    float vertical = dy >= 0.0f ? 1.0f : -1.0f;
    glBegin(GL_TRIANGLES);
    if (std::abs(dx) > std::abs(dy)) {
        glVertex2f(x1, y1);
        glVertex2f(x1 - 14.0f * horizontal, y1 + 7.0f);
        glVertex2f(x1 - 14.0f * horizontal, y1 - 7.0f);
    } else {
        glVertex2f(x1, y1);
        glVertex2f(x1 - 7.0f, y1 - 14.0f * vertical);
        glVertex2f(x1 + 7.0f, y1 - 14.0f * vertical);
    }
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void drawTexturedPanel() {
    glBindTexture(GL_TEXTURE_2D, textureId);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(80.0f, 150.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(360.0f, 150.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(360.0f, 430.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(80.0f, 430.0f);
    glEnd();

    drawArrow(80.0f, 120.0f, 360.0f, 120.0f);
    drawArrow(50.0f, 150.0f, 50.0f, 430.0f);
    drawText(205.0f, 92.0f, "U");
    drawText(25.0f, 285.0f, "V");
    drawText(82.0f, 132.0f, "(0,0)");
    drawText(314.0f, 435.0f, "(1,1)");
    drawText(112.0f, 470.0f, "Imagen 2D / textura checker");
}

void drawTexturedQuad3D() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glViewport(520, 105, 340, 340);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.25f, -1.25f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.25f, -1.25f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.25f,  1.25f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.25f,  1.25f, 0.0f);
    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, 900, 600);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 900, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    drawTexturedPanel();
    drawArrow(400.0f, 290.0f, 500.0f, 290.0f);
    drawText(405.0f, 315.0f, "UV");
    drawTexturedQuad3D();
    glDisable(GL_TEXTURE_2D);

    drawText(540.0f, 470.0f, "Superficie 3D texturizada");
    drawText(215.0f, 35.0f, "Imagen -> coordenadas U/V -> muestreo -> superficie");
    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int, int) {
    if (key == 27 || key == 'q' || key == 'Q') std::exit(0);
}

void special(int key, int, int) {
    if (key == GLUT_KEY_LEFT) rotationY -= 5.0f;
    if (key == GLUT_KEY_RIGHT) rotationY += 5.0f;
    glutPostRedisplay();
}

void initOpenGL() {
    glClearColor(0.045f, 0.05f, 0.065f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    makeTexture();
}

int main(int argc, char** argv) {
    printMappingInfo();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Diagrama UV - textura 2D aplicada a superficie");
    initOpenGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
