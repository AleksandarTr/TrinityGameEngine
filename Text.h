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

    Texture *fontTexture = nullptr;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<charInfo> chars;
    int firstChar = 32;

    float windowWidth, windowHeight;
    float fontWidth, fontHeight;
    int maxHeight = 0, maxWidth = 0;

    TextureInfo info = TextureInfo();
    Mesh *mesh = nullptr;
    bool fixed;
    bool changed = false;

    void generateVertices(int length);

public:
    Text(std::string font, int windowWidth, int windowHeight, bool fixed = true);

    void setLength(int length);

    void setMessage(std::string message, glm::vec3 color, int x, int y, int charHeight);

    void generateMessage(std::string message, float x, float y, glm::vec3 color);

    void draw(bool loadTextures = true);

    void readCharInfo(std::string file);

    Mesh& getMesh();

    ~Text();
};

#endif
