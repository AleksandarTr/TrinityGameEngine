#include <iostream>
#include "Text.h"
#include "Mesh.h"
#include "TextureHandler.h"

Text::Text(std::string font, int windowWidth, int windowHeight, bool fixed)
: windowWidth(windowWidth), windowHeight(windowHeight), fixed(fixed) {
    info.location = font + ".png";
    info.format = "image/png";
    info.wrapS = GL_CLAMP_TO_EDGE;
    info.type = TextTexture;
    fontTexture = new Texture(info);
    TextureHandler::getTextureHandler().loadTexture(info,  fontTexture);
    readCharInfo(font);
}

void Text::generateMessage(std::string message, float x, float y, glm::vec3 color) {
    if(vertices.empty() || vertices.size() / 4 != message.length()) setLength(message.length());
    setMessage(message, color, x, y, 50);
}

void Text::draw(bool loadTextures) {
    if(vertices.empty()) throw std::logic_error("Cannot draw text which has not been generated!");
    if(fixed) {
        if(!fontTexture->getId()) return;
        if(changed) {
            VBO.update(vertices);
            VBO.unbind();
            changed = false;
        }

        VAO.bind();

        TextureHandler::bindTexture(*fontTexture);
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "font"), static_cast<int>(fontTexture->getInfo().type));

        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);

        VAO.unbind();
    }
    else {
        if(changed) {
            mesh->updateMesh(&vertices);
            changed = false;
        }

        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyColor"), true);
        mesh->draw(loadTextures);
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyColor"), false);
    }
}

bool isBigEndian();

void bigToLittleEndian(char *integer) {
    char buffer[4];
    for(int i = 0; i < 4; i++) buffer[3 - i] = integer[i];
    memcpy(integer, buffer, 4);
}

void Text::readCharInfo(std::string file) {
    std::ifstream dimensionReader(file + ".png", std::ios::binary);
    if(!dimensionReader.is_open()) throw std::invalid_argument(file + ".png not found.");
    bool flip = !isBigEndian();
    char buffer[4];
    int temp;

    dimensionReader.seekg(16);
    dimensionReader.read(buffer, 4);
    if(flip) bigToLittleEndian(buffer);
    memcpy(&temp, buffer, 4);
    fontWidth = temp;

    dimensionReader.read(buffer, 4);
    if(flip) bigToLittleEndian(buffer);
    memcpy(&temp, buffer, 4);
    fontHeight = temp;

    dimensionReader.close();

    std::ifstream reader(file + ".sfl");
    if(!reader.is_open()) throw std::invalid_argument(file + ".sfl not found.");

    std::string name;
    reader >> name;
    int fontSize, charHeight;
    reader >> fontSize >> charHeight;
    std::string location;
    reader >> location;
    int charCount;
    reader >> charCount;

    for(int i = 0; i < charCount; i++) {
        charInfo info;
        int ascii;
        reader >> ascii >> info.x >> info.y >> info.width >> info.height >> info.xOffset >> info.yOffset >> info.xAdvance;
        chars.push_back(info);

        maxWidth = std::max(maxWidth, info.width);
        maxHeight = std::max(maxHeight, info.height);
    }
    reader.close();
}

void Text::setLength(int length) {
    if(vertices.size() / 4 == length) return;
    if(length <= 0) std::invalid_argument("Text length cannot be less than 1");
    generateVertices(length);

    if(fixed) {
        VAO.bind();
        VBO.bind(vertices);
        VAO.linkVBO(0, 3, GL_FLOAT, sizeof(Vertex), 0);
        VAO.linkVBO(1, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position));
        VAO.linkVBO(2, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color));
        EBO.bind(indices);

        VAO.unbind();
        VBO.unbind();
        EBO.unbind();
    }
    else {
        delete mesh;
        mesh = new SingleTextureMesh(vertices, indices, GL_TRIANGLES, info);
        mesh->bind();
    }
}

void Text::setMessage(std::string message, glm::vec3 color, int left, int top, int charHeight) {
    if(vertices.size() / 4 < message.length()) throw std::invalid_argument("Message cannot fit in this text");
    float x, y;
    float messageWidth = 0;
    for(char ch : message) messageWidth += chars[ch - firstChar].xAdvance;
    messageWidth -= chars[message[message.length() - 1] - firstChar].xAdvance;
    messageWidth += chars[message[message.length() - 1] - firstChar].width;
    float scale = 1.0f * charHeight / maxHeight;
    messageWidth *= scale;

    if(fixed) {
        x = -1 + left / windowWidth;
        y = 1 - (top + charHeight) / windowHeight;
    }
    else {
        x = -messageWidth / windowWidth / 2;
        y = charHeight / windowHeight / 2;
    }

    for(int i = 0; i < message.size(); i++) {
        charInfo character = chars[message[i] - firstChar];
        Vertex &v1 = vertices[4 * i];
        Vertex &v2 = vertices[4 * i + 1];
        Vertex &v3 = vertices[4 * i + 2];
        Vertex &v4 = vertices[4 * i + 3];

        v1.position = glm::vec3(x, y, 0);
        v2.position = glm::vec3(x + character.width * scale / windowWidth, y, 0);
        v3.position = glm::vec3(x + character.width * scale / windowWidth, y + character.height * scale / windowHeight, 0);
        v4.position = glm::vec3(x, y + character.height * scale / windowHeight, 0);
        x += character.xAdvance * scale / windowWidth;

        v1.color = color;
        v2.color = color;
        v3.color = color;
        v4.color = color;

        //TODO: Use unused vectors of Vertex to adjust texture position in fragment shader
        v1.texPosition = glm::vec3(character.x / fontWidth, 1-(character.y + character.height) / fontHeight, 0);
        v2.texPosition = glm::vec3((character.x + character.width) / fontWidth, 1-(character.y + character.height) / fontHeight, 0);
        v3.texPosition = glm::vec3((character.x + character.width) / fontWidth, 1-character.y / fontHeight, 0);
        v4.texPosition = glm::vec3(character.x / fontWidth, 1-character.y / fontHeight, 0);
    }

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






