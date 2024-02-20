#include "Text.h"
#include "Mesh.h"
#include "TextureHandler.h"

Text::Text(std::string font, Shader &shader, int windowWidth, int windowHeight, bool fixed)
: shader(shader), windowWidth(windowWidth), windowHeight(windowHeight), fixed(fixed) {
    info.location = font + ".png";
    info.format = "image/png";
    info.wrapS = GL_CLAMP_TO_EDGE;
    info.type = TextureType::Text;
    fontTexture = new Texture(info);
    TextureHandler::getTextureHandler().loadTexture(info,  fontTexture);
    readCharInfo(font);
}

void Text::generateMessage(std::string message, float x, float y, glm::vec3 color) {
    if(vertices.empty()) setLength(message.length(), x, y, 50, 20);
    else if(vertices.size() / 4 != message.length()) resetLength(message.length(), x, y, 50, 20);
    setMessage(message, color);
}

void Text::draw() {
    if(vertices.empty()) throw std::logic_error("Cannot draw text which has not been generated!");
    if(fixed) {
        shader.activate();
        VAO.bind();

        TextureHandler::bindTexture(*fontTexture);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "font"), static_cast<int>(fontTexture->getInfo().type));

        glDisable(GL_DEPTH_TEST);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glEnable(GL_DEPTH_TEST);

        VAO.unbind();
    }
    else {
        shader.activate();
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "applyColor"), true);
        mesh->draw();
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "applyColor"), false);
    }
}

void Text::readCharInfo(std::string file) {
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

void Text::setLength(int length, int x, int y, int charHeight, int charWidth) {
    if(!vertices.empty()) throw std::invalid_argument("Text length has already been set.\nUse resetLength instead.");
    if(length <= 0) std::invalid_argument("Text length cannot be less than 1");
    generateVertices(length, x, y, charHeight, charWidth);

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
        mesh = new SingleTextureMesh(vertices, indices, shader, GL_TRIANGLES, info);
        mesh->bind();
    }
}

void Text::setMessage(std::string message, glm::vec3 color) {
    if(vertices.size() / 4 < message.length()) throw std::invalid_argument("Message cannot fit in this text");
    for(int i = 0; i < message.size(); i++) {
        charInfo character = chars[message[i] - firstChar];
        Vertex &v1 = vertices[4 * i];
        Vertex &v2 = vertices[4 * i + 1];
        Vertex &v3 = vertices[4 * i + 2];
        Vertex &v4 = vertices[4 * i + 3];

        v1.color = color;
        v2.color = color;
        v3.color = color;
        v4.color = color;

        float height = fontTexture->getInfo().height;
        float width = fontTexture->getInfo().width;
        //TODO: Use unused vectors of Vertex to adjust texture position in fragment shader
        v1.texPosition = glm::vec3(character.x / width, 1-(character.y + character.height) / height, 0);
        v2.texPosition = glm::vec3((character.x + character.width) / width, 1-(character.y + character.height) / height, 0);
        v3.texPosition = glm::vec3((character.x + character.width) / width, 1-character.y / height, 0);
        v4.texPosition = glm::vec3(character.x / width, 1-character.y / height, 0);
    }

    if(fixed) {
        VBO.update(vertices);
        VBO.unbind();
    }
    else mesh->updateMesh(&vertices);
}

Mesh &Text::getMesh() {
    if(vertices.empty()) throw std::logic_error("Cannot return text mesh which has not been generated!");
    return *mesh;
}

void Text::resetLength(int length, int x, int y, int charHeight, int charWidth) {
    if(vertices.empty()) throw std::invalid_argument("Text length has not been set yet.\nUse setLength instead.");
    if(length <= 0) std::invalid_argument("Text length cannot be less than 1");
    vertices.clear();
    indices.clear();
    generateVertices(length, x, y, charHeight, charWidth);

    if(fixed) {
        VBO.update(vertices);
        VBO.unbind();
    }
    else {
        mesh->updateMesh(&vertices);
    }
}

void Text::generateVertices(int length, int left, int top, int charHeight, int charWidth) {
    float x, y;
    if(fixed) {
        x = -1 + left / windowWidth;
        y = 1 - (top + charHeight) / windowHeight;
    }
    else {
        x = - length * charWidth / windowWidth / 2;
        y = charHeight / windowHeight / 2;
    }

    for(int i = 0; i < length; i++) {
        Vertex v1, v2, v3, v4;

        v1.position = glm::vec3(x, y, 0);
        v2.position = glm::vec3(x + charWidth / windowWidth, y, 0);
        v3.position = glm::vec3(x + charWidth / windowWidth, y + charHeight / windowHeight, 0);
        v4.position = glm::vec3(x, y + charHeight / windowHeight, 0);
        x += charWidth / windowWidth;

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




