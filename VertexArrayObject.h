#ifndef TRINITYENGINE_VERTEXARRAYOBJECT_H
#define TRINITYENGINE_VERTEXARRAYOBJECT_H

#include <glad/glad.h>
#include <cstdlib>

class VertexArrayObject {
    GLuint VAO = 0;

public:
    VertexArrayObject();

    void linkVBO(GLuint layout, std::size_t elemCount, GLenum type, std::size_t stride, std::size_t offset);

    void bind();

    void unbind();

    GLuint getID();

    ~VertexArrayObject();
};

#endif
