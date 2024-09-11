#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <glm/glm.hpp>

void loadFont(const std::string& fontPath);
unsigned int initText();
void setupTextProjection(unsigned int textShaderProgram);
void renderText(unsigned int textShader, std::string text, float x, float y, float scale, glm::vec3 color);

#endif
