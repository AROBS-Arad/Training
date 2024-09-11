#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "triangle.h"
#include "text.h"

// Function prototypes
void processInput(GLFWwindow *window, float &xOffset, float &yOffset, float &zOffset, float &rotationAngle);

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set GLFW to use the OpenGL core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGL Triangle with Text", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Initialize triangle
    unsigned int triangleShaderProgram = initTriangle();
    unsigned int textShaderProgram = initText();

    // Initialize text rendering
    initText();
    loadFont("resources/bradley.ttf");

    float xOffset = 0.0f;
    float yOffset = 0.0f;
    float zOffset = -2.0f;
    float rotationAngle = 0.0f;

    int fps;
    int frameCount = 0;
    auto lastTime = std::chrono::high_resolution_clock::now();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate time for FPS
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;

        frameCount++;
        if (deltaTime.count() >= 1.0f) {
            lastTime = currentTime;
            fps = frameCount;
            frameCount = 0;
        }

        // Process input
        processInput(window, xOffset, yOffset, zOffset, rotationAngle);

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render triangle
        renderTriangle(triangleShaderProgram, xOffset, yOffset, zOffset, rotationAngle);

        // Render text "FPS" at the top-right
        setupTextProjection(textShaderProgram);
        renderText(textShaderProgram, "FPS: " + std::to_string(fps), 900.0f, 50.0f, 0.7f, glm::vec3(0.5f, 0.7f, 0.5f)); // Green color

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    cleanupTriangle();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window, float &xOffset, float &yOffset, float &zOffset, float &rotationAngle) {
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        xOffset += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        xOffset -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        yOffset += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        yOffset -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        zOffset += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        zOffset -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        rotationAngle += 0.5f;  // Rotate counter-clockwise
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        rotationAngle -= 0.5f;  // Rotate clockwise
    }
}
