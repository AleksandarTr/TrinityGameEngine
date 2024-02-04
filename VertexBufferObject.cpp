#include <cstddef>
#include "VertexBufferObject.h"

VertexBufferObject::VertexBufferObject() {
    glGenBuffers(1, &VBO);
}

void VertexBufferObject::bind(std::vector<Vertex> vertices) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void VertexBufferObject::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBufferObject::~VertexBufferObject() {
    glDeleteBuffers(1, &VBO);
}
