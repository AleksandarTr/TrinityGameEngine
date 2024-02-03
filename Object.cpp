#include "Object.h"

Object::Object(GLfloat *vertices, std::size_t vertexCount, GLuint *indices, std::size_t indexCount, Shader &shader) :
BaseObject(vertices, vertexCount, 8, indices, indexCount, shader) {}

void Object::VAOlinkVBO() {
    VAO.linkVBO(0, 3, GL_FLOAT, vertexSize * sizeof(float), 0);
    VAO.linkVBO(2, 2, GL_FLOAT, vertexSize * sizeof(float), 3 * sizeof(float));
    VAO.linkVBO(3, 3, GL_FLOAT, vertexSize * sizeof(float), 5 * sizeof(float));
}

void Object::setLight(Light& light, LightingType type) {
    glm::vec3 lightSource = light.getPosition();
    glUniform3f(glGetUniformLocation(shader.getProgramID(), "lightPos"), lightSource.x, lightSource.y, lightSource.z);
    glUniform1i(glGetUniformLocation(shader.getProgramID(), "lightingType"), static_cast<GLint>(type));
}
