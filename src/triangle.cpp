#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "util.h"
#include "triangle.h"

// Triangle vertices
float triangleVertices[] = {
    0.0f,  0.5f, 0.0f, 
   -0.5f, -0.5f, 0.0f, 
    0.5f, -0.5f, 0.0f  
};

unsigned int triangleVAO, triangleVBO;
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 model;
out float zDepth; // Pass depth (z) to fragment shader
void main()
{
    vec4 pos = model * vec4(aPos, 1.0);
    gl_Position = pos;
    zDepth = pos.z; // Pass the z-coordinate to the fragment shader
})";

const char* fragmentShaderSource = R"(
#version 330 core
in float zDepth; // Depth value from vertex shader
out vec4 FragColor;
void main()
{
    // Darken the red color based on the depth. Clamp zDepth to a reasonable range.
    float darkness = clamp(1.0 - (zDepth + 1.0) / 20.0, 0.0, 1.0); // Adjust range based on depth
    FragColor = vec4(1.0 * darkness, 0.0 * darkness, 0.0 * darkness, 1.0);
})";

unsigned int initTriangle() {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader, "VERTEX");

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader, "FRAGMENT");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkProgramLinking(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    glBindVertexArray(triangleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return shaderProgram;
}

void renderTriangle(unsigned int shaderProgram, float xOffset, float yOffset, float zOffset, float rotationAngle) {
    glUseProgram(shaderProgram);

    // Create perspective projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);

    // Create model matrix with transformations
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(xOffset, yOffset, zOffset));
    model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate around Z axis

    // Combine the perspective projection and model transformation
    glm::mat4 mvp = projection * model;

    // Pass the matrix to the shader
    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void cleanupTriangle() {
    glDeleteVertexArrays(1, &triangleVAO);
    glDeleteBuffers(1, &triangleVBO);
}
