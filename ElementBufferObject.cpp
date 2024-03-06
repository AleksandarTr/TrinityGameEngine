#include "ElementBufferObject.h"

ElementBufferObject::ElementBufferObject() {
    glGenBuffers(1, &EBO);
}

void ElementBufferObject::bind(std::vector<GLuint> &indices, bool overwrite) {
    //Generate new buffer and write to it, but don't delete the old one in case some other mesh is using it
    if(overwrite) glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    //Skip data binding if its already assigned, unless overwriting is specified
    if(!assigned || overwrite) {
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
