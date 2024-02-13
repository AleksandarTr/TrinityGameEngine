#include "Light.h"

#include <utility>

Light::Light(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Shader &shader, glm::vec3 color,
             glm::vec3 direction, LightingType type, TextureInfo diffuseTextures, TextureInfo specularTextures) :
        SingleTextureMesh(vertices, indices, shader, GL_TRIANGLES, diffuseTextures, specularTextures), color(color), type(type), direction(direction) {}

const glm::vec3 &Light::getColor() const {
    return color;
}

void Light::setColor(const glm::vec3 color) {
    Light::color = color;
}

const glm::vec3 &Light::getDirection() const {
    return direction;
}

void Light::setDirection(const glm::vec3 direction) {
    Light::direction = direction;
}

LightingType Light::getType() const {
    return type;
}

void Light::setType(LightingType type) {
    Light::type = type;
}

void Light::initializeOtherFields() {
    glUniform3f(glGetUniformLocation(shader.getProgramID(), "lightColor"), color.x, color.y, color.z);
    glUniform1i(glGetUniformLocation(shader.getProgramID(), "useLightColor"), useLightColor);
}

bool Light::doesUseLightColor() const {
    return useLightColor;
}

void Light::setUseLightColor(bool useLightColor) {
    Light::useLightColor = useLightColor;
}
