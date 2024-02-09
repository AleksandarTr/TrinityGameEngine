#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <utility>

int Mesh::textureSlotAllocator = 0;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader, GLenum drawMode)
: vertices(std::move(vertices)), indices(std::move(indices)), shader(shader), drawMode(drawMode) {}

void Mesh::bind() {
    VAO.bind();
    VBO.bind(vertices);
    VAO.linkVBO(0, 3, GL_FLOAT, sizeof(Vertex), 0);
    VAO.linkVBO(1, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position));
    VAO.linkVBO(2, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color));
    VAO.linkVBO(3, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color) + sizeof(Vertex::texPosition));
    EBO.bind(indices);

    VAO.unbind();
    VBO.unbind();
    EBO.unbind();
}

void Mesh::draw() {
    shader.activate();
    VAO.bind();
    if(movedFlag) {
        movedFlag = false;
        updateTransformation();
    }

    glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(modelTransformation));
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
    initializeOtherFields();
    drawTextures();

    glDrawElements(drawMode, indices.size() * sizeof(Index) / sizeof(GLint), GL_UNSIGNED_INT, 0);
    Texture::unbind();
    VAO.unbind();
}

void Mesh::initializeOtherFields() {}

glm::vec3 Mesh::getScale() const {
    return scale;
}

void Mesh::setScale(glm::vec3 scale) {
    //if(scale == glm::vec3(0)) throw std::invalid_argument("Scale cannot be zero");
    this->scale = scale;
}

void Mesh::updateTransformation() {
    glm::quat rotation = getRotation();
    rotationMatrix = glm::mat4_cast(rotation);

    glm::mat4 translationMat = glm::translate(glm::mat4(1), getPosition());
    glm::mat4 scalingMatrix = glm::scale(glm::mat4(1), scale);

    modelTransformation = translationMat * rotationMatrix * scalingMatrix;
}

void Mesh::setTransformMatrix(glm::mat4 transform) {
    modelTransformation = transform;
}
