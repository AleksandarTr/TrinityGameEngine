#include "Model.h"
#include <algorithm>
#include <glm/gtx/matrix_decompose.hpp>

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

void Model::applyScaling(glm::vec3 scaling) {
    scale.x *= scaling.x;
    scale.y *= scaling.y;
    scale.z *= scaling.z;
}

void Model::applyTransformation(glm::mat4 transform) {
    transformationMatrix = transformationMatrix * transform;

    for(auto model : models) model->applyTransformation(transform);
    for(auto mesh : meshes) {
        mesh->setTransformMatrix(transformationMatrix);
    }
}

Model::Model(std::vector<Mesh*> &meshes) : meshes(std::move(meshes)) {}

void Model::draw() {
    for(auto model : models) model->draw();
    for(auto mesh : meshes) mesh->draw();
}

glm::vec3 Model::getScale() {
    return scale;
}
