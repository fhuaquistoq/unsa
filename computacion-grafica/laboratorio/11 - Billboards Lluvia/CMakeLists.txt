cmake_minimum_required(VERSION 3.21)

project(rain_billboards_lab LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(rain_billboards_lab
    src/main.cpp
    src/app/RainApp.cpp
    src/app/RainScene.cpp
    src/scene/OrbitCamera.cpp
    src/rain/RainSystem.cpp
    src/graphics/ShaderProgram.cpp
    src/graphics/Texture2D.cpp
    src/rendering/GroundRenderer.cpp
    src/rendering/RainRenderer.cpp
    src/utils/OpenGLInit.cpp
    src/utils/DeltaTime.cpp
)

target_include_directories(rain_billboards_lab
    PRIVATE
        src
)

target_link_libraries(rain_billboards_lab
    PRIVATE
        OpenGL::GL
        glfw
        glad::glad
        glm::glm
)
