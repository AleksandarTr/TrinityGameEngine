#include <iostream>
#include "Text.h"
#include "Mesh.h"
#include "TextureHandler.h"

Text::Text(std::string font, int windowWidth, int windowHeight, bool fixed)
: windowWidth(windowWidth), windowHeight(windowHeight), fixed(fixed) {
    //Set optimal texture info for displaying text
    TextureInfo info;
    info.location = font + ".png";
    info.format = "image/png";
    info.wrapS = GL_CLAMP_TO_EDGE;
    info.type = TextTexture;

    mesh = new SingleTextureMesh(vertices, indices, GL_TRIANGLES, info);
    mesh->setAlphaMode(SingleTextureMesh::MaskedTexture);
    mesh->setAlphaCutoff(0.1f);

    readCharInfo(font);
}

void Text::generateMessage(std::string message, float x, float y, glm::vec3 color) {
    //Change message length if necessary
    if(vertices.empty() || vertices.size() / 4 != message.length()) setLength(message.length());
    setMessage(message, color, x, y, 50);
}

void Text::draw(bool loadTextures) {
    if(vertices.empty()) throw std::logic_error("Cannot draw text which has not been generated!");
    //Update vertices if something changed
    if(changed) {
        if(meshBound) mesh->updateMesh(&vertices);
        else {
            mesh->bind();
            meshBound = true;
        }
        changed = false;
    }

    //Apply a solid color onto the text
    glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyColor"), true);
    mesh->draw(loadTextures);
    glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyColor"), false);
}

bool isBigEndian();

void bigToLittleEndian(char *integer) {
    char buffer[4];
    for(int i = 0; i < 4; i++) buffer[3 - i] = integer[i];
    memcpy(integer, buffer, 4);
}

void Text::readCharInfo(std::string file) {
    //Read dimensions directly from the png file
    std::ifstream dimensionReader(file + ".png", std::ios::binary);
    if(!dimensionReader.is_open()) throw std::invalid_argument(file + ".png not found.");
    bool flip = !isBigEndian();
    char buffer[4];
    int temp;

    //Read image width
    dimensionReader.seekg(16);
    dimensionReader.read(buffer, 4);
    if(flip) bigToLittleEndian(buffer);
    memcpy(&temp, buffer, 4);
    fontWidth = temp;

    //Read image height
    dimensionReader.read(buffer, 4);
    if(flip) bigToLittleEndian(buffer);
    memcpy(&temp, buffer, 4);
    fontHeight = temp;

    dimensionReader.close();

    //Character info is stored in an sfl file, until font reading from ttf files is implemented
    std::ifstream reader(file + ".sfl");
    if(!reader.is_open()) throw std::invalid_argument(file + ".sfl not found.");

    //Read some unused values
    std::string name;
    reader >> name;
    int fontSize, charHeight;
    reader >> fontSize >> charHeight;
    std::string location;
    reader >> location;
    int charCount;
    reader >> charCount;

    for(int i = 0; i < charCount; i++) {
        //Load character info stored in specified order in the sfl file
        charInfo info;
        int ascii;
        reader >> ascii >> info.x >> info.y >> info.width >> info.height >> info.xOffset >> info.yOffset >> info.xAdvance;
        chars.insert(std::pair(ascii, info));

        maxWidth = std::max(maxWidth, info.width);
        maxHeight = std::max(maxHeight, info.height);
    }

    for(auto &ch : chars) {
        //Adjust character yOffset to be the offset from the bottom and not the top
        ch.second.yOffset = ch.second.height + ch.second.yOffset - maxHeight;
    }

    reader.close();
}

void Text::setLength(int length) {
    //Check if its necessary to change the length
    if(vertices.size() / 4 == length) return;
    if(length <= 0) throw std::invalid_argument("Text length cannot be less than 1");
    vertices.clear();
    indices.clear();

    generateVertices(length);
    changed = true;
}

void Text::setMessage(std::string message, glm::vec3 color, int left, int top, int charHeight) {
    //Check if there are enough quads to display the message
    if(vertices.size() / 4 < message.length()) throw std::invalid_argument("Message cannot fit in this text");
    //Floats which keep track of the x and y position of the bottom left of the current character
    float x, y;
    //Total message width used to make the coordinate (0,0) the center of the text mesh
    float messageWidth = 0;
    for(char ch : message) messageWidth += chars[ch].xAdvance;
    //Last advance is unnecessary since there will be no other characters, but we need to add the width of the last character
    messageWidth -= chars[message[message.length() - 1]].xAdvance;
    messageWidth += chars[message[message.length() - 1]].width;
    //Scaling factor to transform dimensions expressed in pixels to values between -1 to 1
    float scale = 1.0f * charHeight / maxHeight;
    messageWidth *= scale;

    if(fixed) {
        //Set x and y to the position specified by top and left which are measured in pixels from the window border
        x = -1 + left / windowWidth;
        y = 1 - (top + charHeight) / windowHeight;
    }
    else {
        //Set x and y to make sure the center of the mesh is at (0,0)
        x = -messageWidth / windowWidth / 2;
        y = charHeight / windowHeight / 2;
    }

    for(int i = 0; i < message.size(); i++) {
        //Load character info about each character and get its respective quad
        charInfo &ch = chars.at(message[i]);
        Vertex &v1 = vertices[4 * i];
        Vertex &v2 = vertices[4 * i + 1];
        Vertex &v3 = vertices[4 * i + 2];
        Vertex &v4 = vertices[4 * i + 3];

        v1.position = glm::vec3(x + ch.xOffset * scale / windowWidth, y - ch.yOffset * scale / windowHeight, 0);
        v2.position = glm::vec3(x + (ch.width + ch.xOffset) * scale / windowWidth, y - ch.yOffset * scale / windowHeight, 0);
        v3.position = glm::vec3(x + (ch.width + ch.xOffset) * scale / windowWidth, y + (ch.height - ch.yOffset) * scale / windowHeight, 0);
        v4.position = glm::vec3(x + ch.xOffset * scale / windowWidth, y + (ch.height - ch.yOffset) * scale / windowHeight, 0);
        x += ch.xAdvance * scale / windowWidth;

        v1.color = color;
        v2.color = color;
        v3.color = color;
        v4.color = color;

        //Setting x and y position based on the coordinates of the character specified in the sfl file
        //where the y coordinate has to be subtracted from 1, because y coordinate is given from the top
        v1.texPosition = glm::vec3(ch.x / fontWidth, 1 - (ch.y + ch.height) / fontHeight, 0);
        v2.texPosition = glm::vec3((ch.x + ch.width) / fontWidth, 1 - (ch.y + ch.height) / fontHeight, 0);
        v3.texPosition = glm::vec3((ch.x + ch.width) / fontWidth, 1 - ch.y / fontHeight, 0);
        v4.texPosition = glm::vec3(ch.x / fontWidth, 1 - ch.y / fontHeight, 0);
    }

    //Erasing any empty quads
    for(int i = message.size(); i < vertices.size() / 4; i++) {
        Vertex &v1 = vertices[4 * i];
        Vertex &v2 = vertices[4 * i + 1];
        Vertex &v3 = vertices[4 * i + 2];
        Vertex &v4 = vertices[4 * i + 3];

        v1.position = glm::vec3(0);
        v2.position = glm::vec3(0);
        v3.position = glm::vec3(0);
        v4.position = glm::vec3(0);
    }

    changed = true;
}

Mesh &Text::getMesh() {
    if(vertices.empty()) throw std::logic_error("Cannot return text mesh which has not been generated!");
    return *mesh;
}

void Text::generateVertices(int length) {
    for(int i = 0; i < length; i++) {
        Vertex v1, v2, v3, v4;

        if(!fixed) {
            v1.lightNormal = glm::vec3(0, 0, 1);
            v2.lightNormal = glm::vec3(0, 0, 1);
            v3.lightNormal = glm::vec3(0, 0, 1);
            v4.lightNormal = glm::vec3(0, 0, 1);
        }

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);

        indices.push_back(4 * i);
        indices.push_back(4 * i + 1);
        indices.push_back(4 * i + 3);
        indices.push_back(4 * i + 1);
        indices.push_back(4 * i + 2);
        indices.push_back(4 * i + 3);
    }
}

Text::~Text() {
    delete mesh;
}
