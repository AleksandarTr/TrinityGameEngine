#include "ElementBufferObject.h"

ElementBufferObject::ElementBufferObject() {
    glGenBuffers(1, &EBO);
}

void ElementBufferObject::bind(GLuint *indices, std::size_t size) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void ElementBufferObject::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

ElementBufferObject::~ElementBufferObject() {
    glDeleteBuffers(1, &EBO);
}
