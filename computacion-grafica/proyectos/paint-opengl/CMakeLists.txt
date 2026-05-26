cmake_minimum_required(VERSION 3.20)
project(paint-opengl LANGUAGES C CXX)

# ─── Versión de C++ ─────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ─── Dependencias (FetchContent: se descargan solas) ────────────
include(FetchContent)

# GLFW — manejo de ventana + input
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Dear ImGui — interfaz gráfica para el toolbar
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.6
)
# FetchContent_Populate está deprecado, pero ImGui no tiene CMakeLists.txt
# propio, así que no podemos usar MakeAvailable. Forzamos la política antigua.
cmake_policy(SET CMP0169 OLD)
FetchContent_Populate(imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# GLAD — loader de funciones OpenGL (generado manualmente en glad/)
add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)

# ─── OpenGL ─────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)

# ─── Ejecutable principal ───────────────────────────────────────
add_executable(paint
    src/main.cpp
    src/app.cpp
    src/canvas.cpp
    src/toolbar.cpp
    src/renderer.cpp
    src/shader.cpp
    src/line.cpp
    src/polygon.cpp
    src/brush.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(paint PRIVATE
    include
    glad/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(paint PRIVATE
    glad
    glfw
    OpenGL::GL
)

# ─── Copiar shaders al directorio de build ──────────────────────
add_custom_command(TARGET paint POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets/shaders
        ${CMAKE_BINARY_DIR}/assets/shaders
    COMMENT "Copying shaders to build directory"
)