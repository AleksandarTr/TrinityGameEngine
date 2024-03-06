#ifndef TRINITYENGINE_ELEMENTBUFFEROBJECT_H
#define TRINITYENGINE_ELEMENTBUFFEROBJECT_H

#include <glad/glad.h>
#include <cstddef>
#include <vector>
#include "Structs.h"

class ElementBufferObject {
private:
    GLuint EBO = 0; //Index of the index buffer
    bool assigned = false; //Flag to check if the buffer has been assigned when copying the index buffer to another mesh

public:
    ElementBufferObject();

    void bind(std::vector<GLuint> &indices, bool overwrite = false);

    void unbind();

    ~ElementBufferObject();

    void update(std::vector<GLuint> &indices);
};

#endif
