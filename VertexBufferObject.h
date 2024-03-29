#ifndef TRINITYENGINE_VERTEXBUFFEROBJECT_H
#define TRINITYENGINE_VERTEXBUFFEROBJECT_H

#include <glad/glad.h>
#include <vector>
#include "Structs.h"

class VertexBufferObject {
private:
    GLuint VBO = 0;
    bool assigned = false;

public:
    VertexBufferObject();

    void bind(std::vector<Vertex> &vertices, bool overwrite = false);

    void unbind();

    void update(std::vector<Vertex> &vertices);

    ~VertexBufferObject();
};

#endif
