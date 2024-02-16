#include "Text.h"
#include "Mesh.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

Text::Text(std::string font, Shader &shader) : shader(shader) {
    ttf_buffer = new unsigned char[1<<20];
    fread(ttf_buffer, 1, 1 << 20, fopen(font.c_str(), "rb"));
    stbtt_BakeFontBitmap(ttf_buffer, 0, 64, temp_bitmap, 1024, 1024, 32, 96, cdata);
    delete [] ttf_buffer;

    glGenTextures(1, &fontTexture);
    slot = Mesh::getTextureSlot();
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 1024, 1024, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Text::generateMessage(std::string message, float x, float y, glm::vec3 color) {
    for(int i = 0; i < message.size(); i++) {
        stbtt_aligned_quad q;
        if(message[i] >= 32 && message[i] < 128) stbtt_GetBakedQuad(cdata, 1024,1024, message[i]-32, &x,&y,&q,1);
        else stbtt_GetBakedQuad(cdata, 1024,1024, 0, &x,&y,&q,1);

        Vertex v1, v2, v3, v4;
        v1.position = glm::vec3(q.x0, q.y0, 10);
        v2.position = glm::vec3(q.x1, q.y0, 10);
        v3.position = glm::vec3(q.x1, q.y1, 10);
        v4.position = glm::vec3(q.x0, q.y1, 10);

        v1.color = color;
        v2.color = color;
        v3.color = color;
        v4.color = color;

        v1.texPosition = glm::vec3(q.s0,q.t0, 0);
        v2.texPosition = glm::vec3(q.s1,q.t0, 0);
        v3.texPosition = glm::vec3(q.s1,q.t1, 0);
        v4.texPosition = glm::vec3(q.s0,q.t1, 0);

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);

        indices.push_back(4 * i);
        indices.push_back(4 * i + 1);
        indices.push_back(4 * i + 2);
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

void Text::draw() {
    shader.activate();
    VAO.bind();
    glUniform1i(glGetUniformLocation(shader.getProgramID(), "font"), slot);
    glBindTexture(GL_TEXTURE_2D, fontTexture);

    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);

    Texture::unbind();
    VAO.unbind();
}




