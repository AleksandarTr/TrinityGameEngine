#include "Object.h"

Object::Object(GLfloat *vertices, std::size_t vertexCount, GLuint *indices, std::size_t indexCount, Shader &shader) :
BaseObject(vertices, vertexCount, 9, indices, indexCount, shader) {}

void Object::VAOlinkVBO() {
    VAO.linkVBO(0, 3, GL_FLOAT, vertexSize * sizeof(float), 0);
    VAO.linkVBO(1, 4, GL_FLOAT, vertexSize * sizeof(float), 3 * sizeof(float));
    VAO.linkVBO(2, 2, GL_FLOAT, vertexSize * sizeof(float), 7 * sizeof(float));
}
