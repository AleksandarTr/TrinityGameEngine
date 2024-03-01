#ifndef TRINITYENGINE_ELEMENTBUFFEROBJECT_H
#define TRINITYENGINE_ELEMENTBUFFEROBJECT_H

#include <glad/glad.h>
#include <cstddef>
#include <vector>
#include "Structs.h"

class ElementBufferObject {
private:
    GLuint EBO = 0;
    bool assigned = false;

public:
    ElementBufferObject();

    void bind(std::vector<GLuint> &indices, bool overwrite = false);

    void unbind();

    ~ElementBufferObject();

    void update(std::vector<GLuint> &indices);
};

#endif
