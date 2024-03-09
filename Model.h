#ifndef TRINITYENGINE_MODEL_H
#define TRINITYENGINE_MODEL_H

#include "Mesh.h"

class gltfReader;

class Model : public Movable {
private:
    std::vector<Model*> models = std::vector<Model*>();
    std::vector<Mesh*> meshes = std::vector<Mesh*>();
    glm::mat4 transformationMatrix = glm::mat4(1);

    std::vector<glm::vec3> localPositions;
    bool erased = false; //Variable used to signal to its parent that the data from this model has already been erased

    friend class gltfReader;

    void setLocalPosition();

public:
    Model(std::vector<Mesh*> &meshes);

    Model(const Model&);

    void addModel(Model& model); //add specified model, unless it's already a child model, in which case nothing happens

    void removeModel(Model& model); //remove specified model if its present, otherwise do nothing

    void removeModel(int index = -1); /*remove model at specified index,
    if the index is out of range, nothing happens
    by default removes the last element or if given index -1 */

    void operator+=(Model& model); //operator for addModel

    void operator-=(Model& model); //operator for removeModel

    Model& operator[] (int index); //operator for member access without index checks

    Model& at(int index); //member access with index checks

    int getModelCount(); //Get the number of child models(does not count any children of children)

    Mesh* getMesh(int index); //Get the mesh at specified index

    int getMeshCount(); //Get the number of meshes loaded in the model

    void transform(glm::mat4 transform); //Apply transformation matrix, used manly for loading local transformations from a gltf file

    void move(glm::vec3 direction) override; //Recursive function which transposes all child models and meshes and the given model

    void rotate(glm::quat direction) override; //Recursive function which rotates all child models and meshes and the given model

    void rotate(glm::vec3 direction) override; //Recursive function which rotates all child models and meshes and the given model

    void scale(glm::vec3 scaling) override; //Recursive function which scales all child models and meshes and the given model

    void setScale(glm::vec3 scale) override;

    void draw(bool loadTextures = true); //Recursive function which draws all child models and meshes and the given model

    void update(double timeDelta) override;

    ~Model();
};

#endif
