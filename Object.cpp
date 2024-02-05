#include "Object.h"

#include <utility>

Object::Object(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader, std::vector<std::string> diffuseTextures, std::vector<std::string> specularTextures) :
        Mesh(std::move(vertices), std::move(indices), shader, std::move(diffuseTextures), std::move(specularTextures)) {}

void Object::initializeOtherFields() {
    for(int i = 0; i < lights.size(); i++) {
        glm::vec3 lightSource = lights[i]->getPosition();
        glm::vec3 lightDir = lights[i]->getDirection();
        glm::vec3 lightColor = lights[i]->getColor();

        char l1[20];
        char l2[20];
        char l3[20];
        char l4[20];
        sprintf(l1, "lightPos[%d]", i);
        sprintf(l2, "lightingType[%d]", i);
        sprintf(l3, "lightDir[%d]", i);
        sprintf(l4, "lightColor[%d]", i);

        glUniform3f(glGetUniformLocation(shader.getProgramID(), l1), lightSource.x, lightSource.y,lightSource.z);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), l2),static_cast<GLint>(lights[i]->getType()));
        glUniform3f(glGetUniformLocation(shader.getProgramID(), l3), lightDir.x, lightDir.y, lightDir.z);
        glUniform4f(glGetUniformLocation(shader.getProgramID(), l4), lightColor.x, lightColor.y, lightColor.z,1.0f);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "lightNum"), lights.size());
    }
    glUniform3f(glGetUniformLocation(shader.getProgramID(), "camPos"), camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
}

void Object::addLight(Light &light) {
    if(lights.size() >= 16) throw std::invalid_argument("More than 16 light sources cannot be linked to a single object.");
    lights.push_back(&light);
}

void Object::removeLight(int index) {
    if(index == -1) {
        lights.pop_back();
        return;
    }

    lights.erase(lights.begin() + index);
}
