#ifndef TRIANGLE_H
#define TRIANGLE_H

unsigned int initTriangle();
void renderTriangle(unsigned int shaderProgram, float xOffset, float yOffset, float zOffset, float rotationAngle);
void cleanupTriangle();

#endif
