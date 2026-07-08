#include <iostream>

#include "app/AppConfig.hpp"
#include "app/RainApp.hpp"
#include "utils/OpenGLInit.hpp"

int main()
{
    OpenGLInitResult initResult = initializeOpenGL(
        appconfig::WINDOW_WIDTH,
        appconfig::WINDOW_HEIGHT,
        appconfig::WINDOW_TITLE);

    if (!initResult.ok())
    {
        std::cout << initResult.errorMessage << "\n";
        return -1;
    }

    int exitCode = 0;

    {
        RainApp app;
        exitCode = app.run(initResult.window);
    }

    shutdownOpenGL(initResult.window);
    return exitCode;
}
