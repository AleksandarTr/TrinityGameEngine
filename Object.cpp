#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>

Object::Object(GLfloat *vertices, std::size_t vertexCount, std::size_t vertexSize, GLuint * indices, std::size_t indexCount)
: vertices(vertices), indices(indices), vertexSize(vertexSize), vertexCount(vertexCount), indexCount(indexCount) {}

void Object::bind() {
    VAO.bind();
    VBO.bind(vertices, vertexCount * vertexSize * sizeof(GLfloat));
    VAO.linkVBO(0, 3, GL_FLOAT, vertexSize * sizeof(float), 0);
    VAO.linkVBO(1, 4, GL_FLOAT, vertexSize * sizeof(float), 3 * sizeof(float));
    VAO.linkVBO(2, 2, GL_FLOAT, vertexSize * sizeof(float), 7 * sizeof(float));
    EBO.bind(indices, indexCount * sizeof(GLuint));

    //Unbind VBO AND VAO
    VAO.unbind();
    VBO.unbind();
    EBO.unbind();
}

void Object::draw() {
    VAO.bind();

    glDrawElements(GL_TRIANGLES, indexCount * 3, GL_UNSIGNED_INT, 0);

    VAO.unbind();
}

void Object::rotate(float angle, glm::vec3 normal) {
    glm::mat4 rotationMat(1);
    rotationMat = glm::rotate(rotationMat, angle, normal);

    for(int i = 0; i < vertexCount; i++) {
        glm::vec4 vertex(vertices[i * vertexSize], vertices[i * vertexSize + 1], vertices[i * vertexSize + 2], 1);
        vertex = vertex * rotationMat;
        vertices[i * vertexSize] = vertex.x;
        vertices[i * vertexSize + 1] = vertex.y;
        vertices[i * vertexSize + 2] = vertex.z;
    }
}
