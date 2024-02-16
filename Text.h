#ifndef TRINITYENGINE_TEXT_H
#define TRINITYENGINE_TEXT_H

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include <stb/stb_truetype.h>

class Text {
private:
    VertexArrayObject VAO = VertexArrayObject();
    VertexBufferObject VBO = VertexBufferObject();
    ElementBufferObject EBO = ElementBufferObject();
    Shader& shader;

    GLuint fontTexture;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    int slot;

    unsigned char *ttf_buffer;
    unsigned char temp_bitmap[1024*1024];
    stbtt_bakedchar cdata[96];

public:
    Text(std::string font, Shader &shader);

    void generateMessage(std::string message, float x, float y, glm::vec3 color);

    void draw();
};

#endif
