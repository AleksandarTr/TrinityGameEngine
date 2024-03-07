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

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::unordered_map<int, charInfo> chars; //Hash map to get info for each character if not all characters are present

    float windowWidth, windowHeight;
    float fontWidth, fontHeight; // Width and height of font texture image
    int maxHeight = 0, maxWidth = 0;

    SingleTextureMesh *mesh = nullptr;
    bool meshBound = false;
    bool fixed; // If true, the text will be fixed to the screen, aka 2D, otherwise it will be in 3D space
    bool changed = false; //Bool used to delay vertex updates until the last possible moment

    void generateVertices(int length);

    void readCharInfo(std::string file);

public:
    Text(std::string font, int windowWidth, int windowHeight, bool fixed = true); /*Text object, where
    *font is the font location which should NOT include the extension(eg .png)
    *only pngs are supported
    *windowWidth and windowHeight are the dimensions of the window in which the text is displayed
    *fixed determines if the text will be fixed to the screen(true) or will be rendered in the 3D world(false)*/

    void setLength(int length); //Set the maximum length of a message which can be stored in given text
    void setMessage(std::string message, glm::vec3 color, int x, int y, int charHeight); /*Set the message which the text will display,
 *where the length of the text which was previously set must be equal to or greater than the message length */
    void generateMessage(std::string message, float x, float y, glm::vec3 color); //Set the message which the text will display

    void draw(bool loadTextures = true); //Render the text

    Mesh& getMesh(); //Get the mesh which the text contains if its 3D

    ~Text();
};

#endif
