#ifndef TRINITYENGINE_ELEMENTBUFFEROBJECT_H
#define TRINITYENGINE_ELEMENTBUFFEROBJECT_H

#include <glad/glad.h>
#include <cstddef>
#include <vector>
#include "Structs.h"

class ElementBufferObject {
private:
    GLuint EBO = 0;

public:
    ElementBufferObject();

    void bind(std::vector<Index> indices);

    void unbind();

    ~ElementBufferObject();
};

#endif
