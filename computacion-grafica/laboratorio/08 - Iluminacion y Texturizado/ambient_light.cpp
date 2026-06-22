#include <GL/glut.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>

const GLfloat kLightAmbient[4] = {0.85f, 0.85f, 0.85f, 1.0f};
const GLfloat kMaterialAmbient[4] = {0.25f, 0.45f, 0.95f, 1.0f};
float rotationY = 0.0f;

void printCalculation() {
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Ejemplo: luz ambiente\n";
    std::cout << "Formula: I_ambient = L_ambient * K_ambient\n";
    std::cout << "L_ambient = (" << kLightAmbient[0] << ", " << kLightAmbient[1] << ", " << kLightAmbient[2] << ")\n";
    std::cout << "K_ambient = (" << kMaterialAmbient[0] << ", " << kMaterialAmbient[1] << ", " << kMaterialAmbient[2] << ")\n";
    std::cout << "I_ambient = ("
              << kLightAmbient[0] * kMaterialAmbient[0] << ", "
              << kLightAmbient[1] * kMaterialAmbient[1] << ", "
              << kLightAmbient[2] * kMaterialAmbient[2] << ")\n";
    std::cout << "Observacion: no depende de la normal ni de la posicion de la luz.\n" << std::flush;
}

void drawText(float x, float y, const char* text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (const char* c = text; *c; ++c) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
    glutSolidSphere(1.25, 64, 64);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(width) / height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
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
    glClearColor(0.05f, 0.06f, 0.08f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    const GLfloat zero[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    const GLfloat position[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, kLightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, zero);
    glLightfv(GL_LIGHT0, GL_SPECULAR, zero);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, kMaterialAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
}

int main(int argc, char** argv) {
    printCalculation();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Luz ambiente - componente ambiental");
    initOpenGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
