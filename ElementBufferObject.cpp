#include "ElementBufferObject.h"

ElementBufferObject::ElementBufferObject() {
    glGenBuffers(1, &EBO);
}

void ElementBufferObject::bind(std::vector<GLuint> &indices) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    if(!assigned) {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
        assigned = true;
    }
}

void ElementBufferObject::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

ElementBufferObject::~ElementBufferObject() {
    glDeleteBuffers(1, &EBO);
}

void ElementBufferObject::update(std::vector<GLuint> &indices) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
}
