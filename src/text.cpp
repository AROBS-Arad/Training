#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "util.h"
#include "text.h"

// Text rendering
struct Character {
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

std::map<char, Character> Characters;
unsigned int textVAO, textVBO;


const char* textVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;
uniform mat4 projection;
void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
})";

const char* textFragmentShaderSource = R"(
#version 330 core
uniform sampler2D text;
uniform vec3 textColor;
in vec2 TexCoords;
out vec4 color;
void main()
{    
    float alpha = texture(text, TexCoords).r; // Use the red channel as alpha
    if (alpha < 0.1) {
        discard; // Discard pixels with low alpha values to handle transparency
    }
    color = vec4(textColor, 1.0) * alpha; // Apply color with alpha
})";

void loadFont(const std::string& fontPath) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };

        Characters.insert(std::pair<char, Character>(c, character));
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

unsigned int initText() {
    unsigned int textVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(textVertexShader, 1, &textVertexShaderSource, NULL);
    glCompileShader(textVertexShader);
    checkShaderCompilation(textVertexShader, "VERTEX");

    unsigned int textFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(textFragmentShader, 1, &textFragmentShaderSource, NULL);
    glCompileShader(textFragmentShader);
    checkShaderCompilation(textFragmentShader, "FRAGMENT");

    unsigned int textShaderProgram = glCreateProgram();
    glAttachShader(textShaderProgram, textVertexShader);
    glAttachShader(textShaderProgram, textFragmentShader);
    glLinkProgram(textShaderProgram);
    checkProgramLinking(textShaderProgram);

    glDeleteShader(textVertexShader);
    glDeleteShader(textFragmentShader);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);

    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return textShaderProgram;
}

void setupTextProjection(unsigned int textShaderProgram) {
    glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 768.0f, 0.0f);
    glUseProgram(textShaderProgram);
    unsigned int projLoc = glGetUniformLocation(textShaderProgram, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void renderText(unsigned int textShader, std::string text, float x, float y, float scale, glm::vec3 color) {
    glUseProgram(textShader);
    glUniform3f(glGetUniformLocation(textShader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    for (auto c : text) {
        Character ch = Characters[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - ch.Bearing.y * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },   // Bottom-left (flipped)
            { xpos,     ypos,       0.0f, 0.0f },   // Top-left (flipped)
            { xpos + w, ypos,       1.0f, 0.0f },   // Top-right (flipped)

            { xpos,     ypos + h,   0.0f, 1.0f },   // Bottom-left (flipped)
            { xpos + w, ypos,       1.0f, 0.0f },   // Top-right (flipped)
            { xpos + w, ypos + h,   1.0f, 1.0f }    // Bottom-right (flipped)
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
