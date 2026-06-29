cmake_minimum_required(VERSION 3.21)

project(opengl_starter LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(boids
    src/main.cpp
    src/utils/OpenGLInit.cpp
    src/utils/DeltaTime.cpp
    src/core/Boid.cpp
    src/core/Emulator.cpp
)

target_link_libraries(boids
    PRIVATE
        OpenGL::GL
        glfw
        glad::glad
        glm::glm
)
