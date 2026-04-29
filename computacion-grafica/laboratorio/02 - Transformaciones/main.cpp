#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>

#define WINDOW_SIZE 500

void drawSquare(float centerX, float centerY, float normalizedSize)
{
    float halfSize = normalizedSize / 2.0f;

    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 1.0f, 0.0f);

    glVertex2f(centerX - halfSize, centerY - halfSize);

    glVertex2f(centerX + halfSize, centerY - halfSize);

    glVertex2f(centerX + halfSize, centerY + halfSize);

    glVertex2f(centerX - halfSize, centerY + halfSize);

    glEnd();
}

void drawCircle(float centerX, float centerY, float radius, int numSegments)
{
    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 1.0f, 0.0f);

    for (int i = 0; i < numSegments; i++)
    {
        float angle = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);
        glVertex2f(centerX + x, centerY + y);
    }

    glEnd();
}

void drawCirclesCenter(float centerX, float centerY, float initialRadius, int numCircles, int percentageReduction)
{
    for (int i = 0; i < numCircles; i++)
    {
        float radius = initialRadius * (1.0f - (percentageReduction / 100.0f) * i);
        float restRadius = initialRadius - radius;
        drawCircle(centerX - restRadius, centerY, radius, 100);
    }
}

void drawCirclesConsecutive(float centerX, float centerY, float initialRadius, int numCircles, int percentageReduction)
{
    float currentX = centerX - initialRadius;
    for (int i = 0; i < numCircles; i++)
    {
        float radius = initialRadius * (1.0f - (percentageReduction / 100.0f) * i);
        currentX += radius;
        drawCircle(currentX, centerY, radius, 100);
        currentX += radius;
    }
}

void drawCirclesWithAngle(float centerX, float centerY, float radius, int numSegments, float rotationAngle)
{
    float rotationRadians = rotationAngle * 3.1415926f / 180.0f;
    float cosineRotation = cosf(rotationRadians);
    float sineRotation = sinf(rotationRadians);

    glBegin(GL_LINE_LOOP);
    glColor3f(1.0f, 1.0f, 0.0f);

    for (int i = 0; i < numSegments; i++)
    {
        float angle = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);

        float rotatedX = x * cosineRotation - y * sineRotation;
        float rotatedY = x * sineRotation + y * cosineRotation;

        glVertex2f(centerX + rotatedX, centerY + rotatedY);
    }

    glEnd();
}

int main()
{
    std::cout << "Laboratorio 02: Transformaciones" << std::endl;
    std::cout << "Funciones:" << std::endl;
    std::cout << "1) Dibujar cuadrado" << std::endl;
    std::cout << "2) Dibujar círculo" << std::endl;
    std::cout << "3) Dibujar círculos centro" << std::endl;
    std::cout << "4) Dibujar círculos consecutivos" << std::endl;
    std::cout << "5) Dibujar círculos con angulo" << std::endl;
    std::cout << "0) Salir" << std::endl;

    int option, x, y, size = 0, radius = 0, numSegments, numCircles, percentageReduction, rotationAngle;
    std::cin >> option;

    if (option == 0)
    {
        std::cout << "Saliendo del programa." << std::endl;
        return 0;
    }
    else if (option == 1)
    {
        std::cout << "Dibujar cuadrado seleccionado." << std::endl;
        std::cout << "Ingrese el centro del cuadrado (x y) y el tamaño: ";
        std::cin >> x >> y >> size;
    }
    else if (option == 2)
    {
        std::cout << "Dibujar círculo seleccionado." << std::endl;
        std::cout << "Ingrese el centro del círculo (x y), el radio y el número de segmentos: ";
        std::cin >> x >> y >> radius >> numSegments;
    }
    else if (option == 3)
    {
        std::cout << "Dibujar círculos centro seleccionado." << std::endl;
        std::cout << "Ingrese el centro del círculo (x y), el radio inicial, el número de círculos y el porcentaje de reducción: ";
        std::cin >> x >> y >> radius >> numCircles >> percentageReduction;
    }
    else if (option == 4)
    {
        std::cout << "Dibujar círculos consecutivos seleccionado." << std::endl;
        std::cout << "Ingrese el centro del círculo (x y), el radio inicial, el número de círculos y el porcentaje de reducción: ";
        std::cin >> x >> y >> radius >> numCircles >> percentageReduction;
    }
    else if (option == 5)
    {
        std::cout << "Dibujar círculos con angulo seleccionado." << std::endl;
        std::cout << "Ingrese el centro del círculo (x y), el radio, el número de círculos, el porcentaje de reducción y el ángulo de rotación: ";
        std::cin >> x >> y >> radius >> numCircles >> percentageReduction >> rotationAngle;
    }
    else
    {
        std::cout << "Opción no válida. Saliendo del programa." << std::endl;
        return -1;
    }

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow *window = glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "Triangulo sin shaders", NULL, NULL);
    glfwMakeContextCurrent(window);

    float centerX = (x - WINDOW_SIZE / 2) / (WINDOW_SIZE / 2.0f);
    float centerY = (y - WINDOW_SIZE / 2) / (WINDOW_SIZE / 2.0f);
    float normalizeSize = size / (WINDOW_SIZE / 2.0f);
    float normalizedRadius = radius / (WINDOW_SIZE / 2.0f);

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

        if (option == 1)
            drawSquare(centerX, centerY, normalizeSize);
        else if (option == 2)
            drawCircle(centerX, centerY, normalizedRadius, numSegments);
        else if (option == 3)
            drawCirclesCenter(centerX, centerY, normalizedRadius, numCircles, percentageReduction);
        else if (option == 4)
            drawCirclesConsecutive(centerX, centerY, normalizedRadius, numCircles, percentageReduction);

        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}