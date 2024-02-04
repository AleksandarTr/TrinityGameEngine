#include "Light.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Light::Light(GLfloat *vertices, std::size_t vertexCount, GLuint * indices, std::size_t indexCount, Shader &shader)
        : BaseObject(vertices, vertexCount, 3, indices, indexCount, shader) {}

void Light::VAOlinkVBO() {
    VAO.linkVBO(0, 3, GL_FLOAT, vertexSize * sizeof(float), 0);
}