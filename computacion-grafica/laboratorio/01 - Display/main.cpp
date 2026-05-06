#include <iostream>
#include <GLFW/glfw3.h>

#define CENTER_X 250
#define CENTER_Y 250

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Uso: " << argv[0] << " <center_x> <center_y> <size>" << std::endl;
        return -1;
    }

    int center_x = std::stoi(argv[1]);
    int center_y = std::stoi(argv[2]);
    int size = std::stoi(argv[3]);

    if (center_x < 0 || center_x > 500 || center_y < 0 || center_y > 500 || size <= 0)
    {
        std::cout << "Valores inválidos. Asegúrate de que center_x y center_y estén entre 0 y 500, y size sea positivo." << std::endl;
        return -1;
    }

    glfwInit();

    // Pedimos un contexto viejo (compatibility)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow *window = glfwCreateWindow(500, 500, "Triangulo sin shaders", NULL, NULL);
    glfwMakeContextCurrent(window);

    float centerX = (center_x - CENTER_X) / 250.0f;
    float centerY = ((500 - center_y) - CENTER_Y) / 250.0f;
    // float centerY = (center_y - CENTER_Y) / 250.0f;
    float halfSize = size / 500.0f;

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            centerY += 0.01f;
        else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            centerY -= 0.01f;
        else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            centerX -= 0.01f;
        else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            centerX += 0.01f;

        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_LINE_LOOP);
        glColor3f(1.0f, 1.0f, 0.0f);

        glVertex2f(centerX - halfSize, centerY - halfSize);

        glVertex2f(centerX + halfSize, centerY - halfSize);

        glVertex2f(centerX + halfSize, centerY + halfSize);

        glVertex2f(centerX - halfSize, centerY + halfSize);

        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}