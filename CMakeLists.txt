cmake_minimum_required(VERSION 3.20)
project(OpenGLApp)

set(CMAKE_CXX_STANDARD 17)

# GLFW
add_subdirectory(external/glfw)

# GLAD
add_library(glad external/glad/src/glad.c "include/stb_image.h")
target_include_directories(glad PUBLIC external/glad/include)

# Executables
# add_executable(concentric_triangles concentric_triangles.cpp "include/stb_image.h")
add_executable(balls balls.cpp "include/stb_image.h")

# Include directories
target_include_directories(balls PRIVATE
    include
    external/glad/include
    external/glfw/include
)

# Link libraries
# target_link_libraries(concentric_triangles glfw glad opengl32)
target_link_libraries(balls glfw glad opengl32)