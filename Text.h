#ifndef TRINITYENGINE_TEXT_H
#define TRINITYENGINE_TEXT_H

#include <unordered_map>
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include "Texture.h"
#include "SingleTextureMesh.h"

class Text {
private:
    struct charInfo {
        int x, y; //x and y position of the character in the image file
        int width, height; // width and height of the character in the image file
        int xOffset, yOffset; //x and y offset of characters from top-left when displaying them
        int xAdvance; //how much to move to the right to display the next character
    };

    //TODO: Remove buffers from text
    VertexArrayObject VAO = VertexArrayObject();
    VertexBufferObject VBO = VertexBufferObject();
    ElementBufferObject EBO = ElementBufferObject();

    Texture *fontTexture = nullptr;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::unordered_map<int, charInfo> chars; //Hash map to get info for each character if not all characters are present

    float windowWidth, windowHeight;
    float fontWidth, fontHeight; // Width and height of font texture image
    int maxHeight = 0, maxWidth = 0;

    SingleTextureMesh *mesh = nullptr;
    bool fixed; // If true, the text will be fixed to the screen, aka 2D, otherwise it will be in 3D space
    bool changed = false; //Bool used to delay vertex updates until the last possible moment

    void generateVertices(int length);

    void readCharInfo(std::string file);

public:
    Text(std::string font, int windowWidth, int windowHeight, bool fixed = true); //Font location should NOT include the extension

    void setLength(int length);
    void setMessage(std::string message, glm::vec3 color, int x, int y, int charHeight);
    void generateMessage(std::string message, float x, float y, glm::vec3 color);

    void draw(bool loadTextures = true);

    Mesh& getMesh();

    void setDoubleSided(bool value);

    ~Text();
};

#endif
