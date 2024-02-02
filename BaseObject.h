#ifndef TRINITYENGINE_BASE_OBJECT_H
#define TRINITYENGINE_BASE_OBJECT_H

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include <glm/glm.hpp>

class BaseObject {
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

    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);

public:
    BaseObject(GLfloat *vertices, std::size_t vertexCount, std::size_t vertexSize, GLuint * indices, std::size_t indexCount, Shader &shader);

    virtual void VAOlinkVBO() = 0;

    void rotate(glm::vec3 angles);

    void move(glm::vec3 direction);

    void bind();

    void draw();

    glm::vec3 getPosition();
};

#endif
