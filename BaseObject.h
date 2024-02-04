#ifndef TRINITYENGINE_BASE_OBJECT_H
#define TRINITYENGINE_BASE_OBJECT_H

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include "MoveableObject.h"
#include <glm/glm.hpp>

class BaseObject : public MoveableObject {
public:
    VertexArrayObject VAO;
    VertexBufferObject VBO;
    ElementBufferObject EBO;
    Shader &shader;

    GLfloat *vertices;
    std::size_t vertexCount;
    std::size_t vertexSize;
    GLuint *indices;
    std::size_t indexCount;

public:
    BaseObject(GLfloat *vertices, std::size_t vertexCount, std::size_t vertexSize, GLuint * indices, std::size_t indexCount, Shader &shader);

    virtual void VAOlinkVBO() = 0;

    void bind();

    void draw();
};

#endif
