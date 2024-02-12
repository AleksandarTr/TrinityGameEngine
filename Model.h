#ifndef TRINITYENGINE_MODEL_H
#define TRINITYENGINE_MODEL_H

#include "Mesh.h"

class gltfReader;

class Model : private Movable {
private:
    std::vector<Model*> models = std::vector<Model*>();
    std::vector<Mesh*> meshes = std::vector<Mesh*>();
    glm::mat4 transformationMatrix = glm::mat4(1);
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

    void move(glm::vec3 direction);

    void rotate(glm::quat direction);

    void rotate(glm::vec3 direction);

    void scale(glm::vec3 scaling);

    void draw();

    using Movable::getScale;
    using Movable::getRotation;
    using Movable::getPosition;
};

#endif
