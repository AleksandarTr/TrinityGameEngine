#ifndef TRINITYENGINE_MODEL_H
#define TRINITYENGINE_MODEL_H

#include "Mesh.h"
#include "Scene.h"

class Model : public Movable {
private:
    std::vector<Model*> models = std::vector<Model*>();
    std::vector<Mesh*> meshes = std::vector<Mesh*>();
    glm::vec3 scale = glm::vec3(1);
    glm::mat4 transformationMatrix = glm::mat4(1);

public:
    Model(std::vector<Mesh*> &meshes);

    void addModel(Model& model);

    void removeModel(Model& model);

    void removeModel(int index = -1);

    void operator+=(Model& model);

    void operator-=(Model& model);

    Model& operator[] (int index);

    Model& at(int index);

    void applyScaling(glm::vec3 scaling);

    void applyTransformation(glm::mat4 transform);

    void draw();

    glm::vec3 getScale();
};

#endif
