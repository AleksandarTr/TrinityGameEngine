#include "Model.h"
#include <algorithm>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>

Model &Model::operator[](int index) {
    return *models[index];
}

Model &Model::at(int index) {
    if(models.size() > index) return *models[index];
    else throw std::out_of_range("Index of model is out of range of existing models in this scene.");
}

void Model::addModel(Model &model) {
    if(std::find(models.begin(), models.end(), &model) != models.end()) return;
    models.push_back(&model);
}

void Model::removeModel(Model &model) {
    auto modelAdr = std::find(models.begin(), models.end(), &model);
    if(modelAdr != models.end()) models.erase(modelAdr);
}

void Model::removeModel(int index) {
    if(models.empty() || models.size() > index) return;
    if(index == -1) models.pop_back();
    else models.erase(models.begin() + index);
}

void Model::operator+=(Model &model) {
    addModel(model);
}

void Model::operator-=(Model &model) {
    removeModel(model);
}

void Model::transform(glm::mat4 transform) {
    transformationMatrix *= transform;

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transformationMatrix, scale, rotation, position, skew, perspective);

    setPosition(position);
    setRotation(rotation);
    setScale(scale);

    for(auto model : models) model->transform(transform);
    for(auto mesh : meshes) {
        mesh->setPosition(position);
        mesh->setRotation(rotation);
        mesh->setScale(scale);
    }
}

Model::Model(std::vector<Mesh*> &meshes) : meshes(meshes) {}

void Model::draw() {
    for(auto model : models) model->draw();
    for(auto mesh : meshes) mesh->draw();
}

void Model::move(glm::vec3 direction) {
    Movable::move(direction);

    for(auto model : models) model->move(direction);
    for(auto mesh : meshes) mesh->setPosition(getPosition());
}

void Model::rotate(glm::vec3 direction) {
    Movable::rotate(direction);
    glm::vec3 newPos = glm::eulerAngleXYZ(direction.x, direction.y, direction.z) * glm::vec4(localPosition, 1.0f);
    Movable::setPosition(getPosition() - localPosition + newPos);
    localPosition = newPos;

    for(auto model : models) model->rotate(direction);
    for(auto mesh : meshes) {
        mesh->setRotation(getRotation());
        mesh->setPosition(getPosition());
    }
}

void Model::rotate(glm::quat direction) {
    Movable::rotate(direction);
    glm::vec3 newPos = glm::toMat4(direction) * glm::vec4(localPosition, 1.0f);
    Movable::setPosition(getPosition() - localPosition + newPos);
    localPosition = newPos;

    for(auto model : models) model->rotate(direction);
    for(auto mesh : meshes) {
        mesh->setRotation(getRotation());
        mesh->setPosition(getPosition());
    }
}

void Model::scale(glm::vec3 scaling) {
    Movable::scale(scaling);
    glm::vec3 newPos = localPosition * scaling;
    Movable::setPosition(getPosition() - localPosition + newPos);
    localPosition = newPos;

    for(auto model : models) model->scale(scaling);
    for(auto mesh : meshes) {
        mesh->setScale(getScale());
        mesh->setPosition(getPosition());
    }
}

void Model::setLocalPosition() {
    localPosition = getPosition();
    for(auto model : models) model->setLocalPosition();
}