#include "Model.h"
#include "SingleTextureMesh.h"
#include <algorithm>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <set>
#include <stack>
#include <iostream>

Model &Model::operator[](int index) {
    return *models[index];
}

Model &Model::at(int index) {
    if(models.size() > index || index < 0) return *models[index];
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
    if(models.size() > index || index < -1) return;
    if(index == -1) models.pop_back(); //Remove last element if the function is called without arguments
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

    //transformations in the meshes are stored separately as translation, rotation and scale,
    // so the final transformation matrix has to be decomposed into them
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(transformationMatrix, scale, rotation, position, skew, perspective);

    setPosition(position);
    setRotation(glm::eulerAngles(rotation));
    setScale(scale);

    for(auto model : models) model->transform(transform);
    for(auto mesh : meshes) {
        mesh->setPosition(position);
        mesh->setRotation(glm::eulerAngles(rotation));
        mesh->setScale(scale);
    }
}

Model::Model(std::vector<Mesh*> &meshes) : meshes(meshes) {}

void Model::draw(bool loadTextures) {
    //Recursively call all the child models, until all of them have been drawn
    std::stack<Model*> stack;
    stack.push(this);

    while(!stack.empty()) {
        Model *currentModel = stack.top();
        stack.pop();

        for (auto model: currentModel->models) stack.push(model);
        for (auto mesh: currentModel->meshes) mesh->draw(loadTextures);
    }
}

void Model::move(glm::vec3 direction) {
    //Translation does not change the local position within the predecessor models
    std::stack<Model*> stack;
    stack.push(this);

    while(!stack.empty()) {
        Model* currentModel = stack.top();
        stack.pop();

        currentModel->Movable::move(direction);
        for (auto model: currentModel->models) stack.push(model);
        for (auto mesh: currentModel->meshes) mesh->setPosition(currentModel->getPosition());
    }
}

void Model::rotate(glm::vec3 direction) {
    struct StackElement {
        Model* model;
        int depth;
    };

    std::stack<StackElement> stack;
    stack.push({this, 0});
    glm::mat4 rotationMatrix = glm::eulerAngleXYZ(direction.x, direction.y, direction.z);

    while(!stack.empty()) {
        Model *currentModel = stack.top().model;
        int currentDepth = stack.top().depth;
        stack.pop();

        currentModel->Movable::rotate(direction);
        //All the local positions within the predecessor models have to be rotated
        for(int i = 0; i < currentDepth; i++) {
            glm::vec3 newPos = rotationMatrix * glm::vec4(currentModel->localPositions[i], 1.0f);
            currentModel->Movable::move(newPos - currentModel->localPositions[i]);
            currentModel->localPositions[i] = newPos;
        }

        currentDepth++;
        for (auto model: currentModel->models) stack.push({model, currentDepth});
        for (auto mesh: currentModel->meshes) {
            mesh->setRotation(glm::eulerAngles(currentModel->getRotation()));
            mesh->setPosition(currentModel->getPosition());
        }
    }
}

void Model::rotate(glm::quat direction) {
    struct StackElement {
        Model* model;
        int depth;
    };

    std::stack<StackElement> stack;
    stack.push({this, 0});
    glm::mat4 rotationMatrix = glm::toMat4(direction);

    while(!stack.empty()) {
        Model *currentModel = stack.top().model;
        int currentDepth = stack.top().depth;
        stack.pop();

        currentModel->Movable::rotate(direction);
        //All the local positions within the predecessor models have to be rotated
        for(int i = 0; i < currentDepth; i++) {
            glm::vec3 newPos = rotationMatrix * glm::vec4(currentModel->localPositions[i], 1.0f);
            currentModel->Movable::move(newPos - currentModel->localPositions[i]);
            currentModel->localPositions[i] = newPos;
        }

        currentDepth++;
        for (auto model: currentModel->models) stack.push({model, currentDepth});
        for (auto mesh: currentModel->meshes) {
            mesh->setRotation(currentModel->getRotation());
            mesh->setPosition(currentModel->getPosition());
        }
    }
}

void Model::scale(glm::vec3 scaling) {
    struct StackElement {
        Model *model;
        int depth;
    };

    std::stack<StackElement> stack;
    stack.push({this, 0});

    while(!stack.empty()) {
        Model *currentModel = stack.top().model;
        int currentDepth = stack.top().depth;
        stack.pop();

        currentModel->Movable::scale(scaling);
        //All the local positions within the predecessor models have to be scaled
        for(int i = 0; i < currentDepth; i++) {
            glm::vec3 newPos = currentModel->localPositions[i] * scaling;
            currentModel->Movable::setPosition(currentModel->getPosition() - currentModel->localPositions[i] + newPos);
            currentModel->localPositions[i] = newPos;
        }

        currentDepth++;
        for (auto model: currentModel->models) stack.push({model, currentDepth});
        for (auto mesh: currentModel->meshes) {
            mesh->setScale(currentModel->getScale());
            mesh->setPosition(currentModel->getPosition());
        }
    }
}

void Model::setLocalPosition() {
    //Used to send the local position when the final local transformation of the model has been loaded
    std::stack<Model*> stack;
    stack.push(this);
    while(!stack.empty()) {
        Model* currentModel = stack.top();
        stack.pop();
        currentModel->localPositions.push_back(getPosition());
        for (auto model: currentModel->models) stack.push(model);
    }
}

Model::Model(const Model &copy) {
    //Copy all child models
    for(Model* model : copy.models) {
        Model &modelCopy = *new Model(*model);
        addModel(modelCopy);
    }

    //Copy all meshes
    for(Mesh* mesh : copy.meshes) {
        Mesh *meshCopy = new SingleTextureMesh(*dynamic_cast<SingleTextureMesh*>(mesh));
        meshes.push_back(meshCopy);
        meshCopy->bind();
    }
}

Model::~Model() {
    if(erased) return; //Prevent the model from getting deleted multiple times
    //The root of the model tree must be called for deleting the whole tree
    //If destructor is called on some child node, the whole tree will not be able to be deleted
    std::set<std::vector<Vertex>*> vertexVectors;
    std::set<std::vector<GLuint>*> indexVectors;

    std::stack<Model*> s;
    s.push(this);

    while(!s.empty()) {
        Model* model = s.top();
        s.pop();

        for(Model *model : model->models) s.push(model);
        for(Mesh* mesh : model->meshes) {
            vertexVectors.insert(&(mesh->vertices));
            indexVectors.insert(&(mesh->indices));
            delete mesh;
        }

        model->erased = true;
        if(model != this) delete model;
    }

    for(std::vector<Vertex>* vec : vertexVectors) delete vec;
    for(std::vector<GLuint>* vec : indexVectors) delete vec;
}

Mesh *Model::getMesh(int index) {
    return meshes.at(index);
}

int Model::getMeshCount() {
    return meshes.size();
}

int Model::getModelCount() {
    return models.size();
}

void Model::update(double timeDelta) {
    std::stack<Model*> stack;
    stack.push(this);

    while(!stack.empty()) {
        Model *currentModel = stack.top();
        stack.pop();
        currentModel->Movable::update(timeDelta);
        for(Model *model : currentModel->models) stack.push(model);
    }
}

void Model::setScale(glm::vec3 scale) {
    this->scale(scale / getScale());
}
