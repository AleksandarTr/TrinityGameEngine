#ifndef TRINITYENGINE_TEXT_H
#define TRINITYENGINE_TEXT_H

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include "Texture.h"
#include "SingleTextureMesh.h"

class Text {
private:
    struct charInfo {
        int x, y;
        int width, height;
        int xOffset, yOffset;
        int xAdvance;
    };

    VertexArrayObject VAO = VertexArrayObject();
    VertexBufferObject VBO = VertexBufferObject();
    ElementBufferObject EBO = ElementBufferObject();
    Shader& shader;

    Texture *fontTexture = nullptr;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<charInfo> chars;
    int firstChar = 32;

    float windowWidth, windowHeight;
    int maxHeight = 0, maxWidth = 0;

    TextureInfo info = TextureInfo();
    Mesh *mesh = nullptr;
    bool fixed;

    void generateVertices(int length, int x, int y, int charHeight, int charWidth);

public:
    Text(std::string font, Shader &shader, int windowWidth, int windowHeight, bool fixed = true);

    void setLength(int length, int x, int y, int charHeight, int charWidth);

    void resetLength(int length, int x, int y, int charHeight, int charWidth);

    void setMessage(std::string message, glm::vec3 color);

    void generateMessage(std::string message, float x, float y, glm::vec3 color);

    void draw();

    void readCharInfo(std::string file);

    Mesh& getMesh();
};

#endif
