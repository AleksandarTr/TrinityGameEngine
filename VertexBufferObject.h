#ifndef TRINITYENGINE_VERTEXBUFFEROBJECT_H
#define TRINITYENGINE_VERTEXBUFFEROBJECT_H

#include <glad/glad.h>

class VertexBufferObject {
private:
    GLuint VBO;

public:
    VertexBufferObject();

    void bind(GLfloat* vertices, std::size_t size);

    void unbind();

    ~VertexBufferObject();
};

#endif
