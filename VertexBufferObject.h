#ifndef TRINITYENGINE_VERTEXBUFFEROBJECT_H
#define TRINITYENGINE_VERTEXBUFFEROBJECT_H

#include <glad/glad.h>
#include <vector>
#include "Structs.h"

class VertexBufferObject {
private:
    GLuint VBO = 0;

public:
    VertexBufferObject();

    void bind(std::vector<Vertex> vertices);

    void unbind();

    ~VertexBufferObject();
};

#endif
