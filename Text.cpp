#include "Text.h"
#include "Mesh.h"

Text::Text(std::string font, Shader &shader, int windowWidth, int windowHeight)
: shader(shader), windowWidth(windowWidth), windowHeight(windowHeight) {
    TextureInfo info;
    info.location = font + ".png";
    info.format = "image/png";
    fontTexture = new Texture(info, Mesh::getTextureSlot());
    readCharInfo(font);
}

void Text::generateMessage(std::string message, float x, float y, glm::vec3 color) {
    setLength(message.length(), x, y, 50, 20);
    setMessage(message, color);
}

void Text::draw() {
    shader.activate();
    VAO.bind();
    glUniform1i(glGetUniformLocation(shader.getProgramID(), "font"), fontTexture->getSlot());
    fontTexture->bind();

    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);

    Texture::unbind();
    VAO.unbind();
}

void Text::readCharInfo(std::string file) {
    std::ifstream reader(file + ".sfl");
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

void Text::setLength(int length, int left, int top, int charHeight, int charWidth) {
    float x = -1 + left / windowWidth;
    float y = 1 - (top + charHeight) / windowHeight;

    for(int i = 0; i < length; i++) {
        Vertex v1, v2, v3, v4;

        v1.position = glm::vec3(x, y, 0);
        v2.position = glm::vec3(x + charWidth / windowWidth, y, 0);
        v3.position = glm::vec3(x + charWidth / windowWidth, y + charHeight / windowHeight, 0);
        v4.position = glm::vec3(x, y + charHeight / windowHeight, 0);
        x += charWidth / windowWidth;

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

void Text::setMessage(std::string message, glm::vec3 color) {
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

        float height = fontTexture->getHeight();
        float width = fontTexture->getWidth();
        v1.texPosition = glm::vec3(character.x / width, 1-(character.y + character.height) / height, 0);
        v2.texPosition = glm::vec3((character.x + character.width) / width, 1-(character.y + character.height) / height, 0);
        v3.texPosition = glm::vec3((character.x + character.width) / width, 1-character.y / height, 0);
        v4.texPosition = glm::vec3(character.x / width, 1-character.y / height, 0);
    }

    VBO.update(vertices);
    VBO.unbind();
}




