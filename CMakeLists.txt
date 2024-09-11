cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(Simple3DApp VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the required packages (GLFW, OpenGL)
find_package(OpenGL REQUIRED)

# Add the GLFW library path
add_subdirectory(external/glfw)

# Add the GLM library path
add_subdirectory(external/glm)

# Add the FreeType library path
add_subdirectory(external/freetype)

# Include the GLAD library files
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# Add the executable
file(GLOB SOURCES "src/*.cpp")
add_executable(Simple3DApp ${SOURCES})

# Link the libraries (GLFW, OpenGL, GLAD)
target_link_libraries(Simple3DApp glfw OpenGL::GL glad glm freetype)

# Set include directories
target_include_directories(Simple3DApp PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/glfw/include
    ${CMAKE_SOURCE_DIR}/external/glm/include
    ${CMAKE_SOURCE_DIR}/external/freetype/include
)
