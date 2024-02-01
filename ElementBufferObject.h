#ifndef TRINITYENGINE_ELEMENTBUFFEROBJECT_H
#define TRINITYENGINE_ELEMENTBUFFEROBJECT_H

#include <glad/glad.h>
#include <cstddef>

class ElementBufferObject {
private:
    GLuint EBO;

public:
    ElementBufferObject();

    void bind(GLuint* indices, std::size_t size);

    void unbind();

    ~ElementBufferObject();
};

#endif
