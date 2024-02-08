#ifndef TRINITYENGINE_MODEL_H
#define TRINITYENGINE_MODEL_H

#include "Mesh.h"
#include "Scene.h"

class Model : public Movable {
private:
    std::vector<Model*> models;
    Mesh *mesh = nullptr;
    glm::vec3 scale;

public:
    Model() = default;

    void addModel(Model& model);

    void removeModel(Model& model);

    void removeModel(int index = -1);

    void operator+=(Model& model);

    void operator-=(Model& model);

    Model& operator[] (int index);

    Model& at(int index);

    void applyScaling(glm::vec3 scaling);

    void applyTransformation(glm::vec3 translation, glm::quat rotation, glm::vec3 scaling);
};

#endif
