#ifndef TRINITYENGINE_LIGHT_H
#define TRINITYENGINE_LIGHT_H

#include <glm/glm.hpp>
#include "BaseObject.h"

class Light : public BaseObject {
private:
    void VAOlinkVBO() override;

public:
    Light(GLfloat *vertices, std::size_t verticesCount, GLuint * indices, std::size_t indicesCount, Shader &shader);
};

#endif
