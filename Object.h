#ifndef TRINITYENGINE_OBJECT_H
#define TRINITYENGINE_OBJECT_H

#include "BaseObject.h"

class Object : public BaseObject {
private:
    void VAOlinkVBO() override;

public:
    Object(GLfloat *vertices, std::size_t vertexCount, GLuint * indices, std::size_t indexCount, Shader &shader);
};


#endif