#include "Scene.h"
#include <algorithm>
#include <stdexcept>

Model &Scene::operator[](int index) {
    return *models[index];
}

Model &Scene::at(int index) {
    if(models.size() > index) return *models[index];
    else throw std::out_of_range("Index of model is out of range of existing models in this scene.");
}

void Scene::addModel(Model &model) {
    if(std::find(models.begin(), models.end(), &model) != models.end()) return;
    models.push_back(&model);
}

void Scene::removeModel(Model &model) {
    auto modelAdr = std::find(models.begin(), models.end(), &model);
    if(modelAdr != models.end()) models.erase(modelAdr);
}

void Scene::removeModel(int index) {
    if(models.empty() || models.size() > index) return;
    if(index == -1) models.pop_back();
    else models.erase(models.begin() + index);
}

void Scene::operator+=(Model &model) {
    addModel(model);
}

void Scene::operator-=(Model &model) {
    removeModel(model);
}
