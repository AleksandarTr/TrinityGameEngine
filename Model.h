#ifndef TRINITYENGINE_MODEL_H
#define TRINITYENGINE_MODEL_H

#include "Mesh.h"

class gltfReader;

class Model : public Movable {
private:
    std::vector<Model*> models = std::vector<Model*>();
    std::vector<Mesh*> meshes = std::vector<Mesh*>();
    glm::mat4 transformationMatrix = glm::mat4(1);

    //TODO: Make transformations on each depth of the model tree, eg. scene[0] should rotate around its own axis
    glm::vec3 localPosition = glm::vec3(0);

    friend class gltfReader;

    void setLocalPosition();

public:
    Model(std::vector<Mesh*> &meshes);

    void addModel(Model& model);

    void removeModel(Model& model);

    void removeModel(int index = -1);

    void operator+=(Model& model);

    void operator-=(Model& model);

    Model& operator[] (int index);

    Model& at(int index);

    void transform(glm::mat4 transform);

    void move(glm::vec3 direction) override;

    void rotate(glm::quat direction) override;

    void rotate(glm::vec3 direction) override;

    void scale(glm::vec3 scaling) override;

    void draw();
};

#endif
