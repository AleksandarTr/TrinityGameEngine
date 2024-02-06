#include "Light.h"

#include <utility>

Light::Light(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader, glm::vec3 color,
             glm::vec3 direction, LightingType type, std::vector<std::string> diffuseTextures,
             std::vector<std::string> specularTextures) :
        MultiTextureMesh(std::move(vertices), std::move(indices), shader, std::move(diffuseTextures), std::move(specularTextures)), color(color), type(type), direction(direction) {}

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
