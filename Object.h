#ifndef TRINITYENGINE_OBJECT_H
#define TRINITYENGINE_OBJECT_H

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include <glm/glm.hpp>

class Object {
private:
    VertexArrayObject VAO;
    VertexBufferObject VBO;
    ElementBufferObject EBO;

    GLfloat *vertices;
    std::size_t vertexCount;
    std::size_t vertexSize;
    GLuint *indices;
    std::size_t indexCount;

public:
    Object(GLfloat *vertices, std::size_t verticesCount, std::size_t vertexSize, GLuint * indices, std::size_t indicesCount);

    void rotate(float angle, glm::vec3 normal, Shader &shader, const char *uniform);

    void bind();

    void draw();
};

#endif
