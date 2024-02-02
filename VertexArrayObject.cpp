#include <cstddef>
#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject() {
    glGenVertexArrays(1, &VAO);
}

VertexArrayObject::~VertexArrayObject() {
    glDeleteVertexArrays(1, &VAO);
}

GLuint VertexArrayObject::getID() {
    return VAO;
}

void VertexArrayObject::bind() {
    glBindVertexArray(VAO);
}

void VertexArrayObject::unbind() {
    glBindVertexArray(0);
}

void VertexArrayObject::linkVBO(GLuint layout, std::size_t elemCount, GLenum type, std::size_t stride, std::size_t offset) {
    glVertexAttribPointer(layout, elemCount, type, GL_FALSE, stride, (void*) offset);
    glEnableVertexAttribArray(layout);
}
