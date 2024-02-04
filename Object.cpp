#include "Object.h"

#include <utility>

Object::Object(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader, std::vector<std::string> diffuseTextures, std::vector<std::string> specularTextures) :
        Mesh(std::move(vertices), std::move(indices), shader, std::move(diffuseTextures), std::move(specularTextures)) {}

void Object::initializeOtherFields() {
    //TODO: Change default.frag to support multiple light sources

    for(auto &light : lights) {
        glm::vec3 lightSource = light->getPosition();
        glm::vec3 lightDir = light->getDirection();
        glm::vec3 lightColor = light->getColor();

        glUniform3f(glGetUniformLocation(shader.getProgramID(), "lightPos"), lightSource.x, lightSource.y,lightSource.z);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "lightingType"),static_cast<GLint>(light->getType()));
        glUniform3f(glGetUniformLocation(shader.getProgramID(), "lightDir"), lightDir.x, lightDir.y, lightDir.z);
        glUniform4f(glGetUniformLocation(shader.getProgramID(), "lightColor"), lightColor.x, lightColor.y, lightColor.z,1.0f);
    }
    glUniform3f(glGetUniformLocation(shader.getProgramID(), "camPos"), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
}

void Object::addLight(Light &light) {
    lights.push_back(&light);
}

void Object::removeLight(int index) {
    if(index == -1) {
        lights.pop_back();
        return;
    }

    lights.erase(lights.begin() + index);
}
