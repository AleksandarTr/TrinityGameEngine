#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, GLenum drawMode)
: vertices(vertices), indices(indices), drawMode(drawMode) {}

void Mesh::bind() {
    VAO.bind();
    VBO.bind(vertices);
    VAO.linkVBO(0, 3, GL_FLOAT, sizeof(Vertex), 0);
    VAO.linkVBO(1, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position));
    VAO.linkVBO(2, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color));
    VAO.linkVBO(3, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color) + sizeof(Vertex::texPosition));
    VAO.linkVBO(4, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color) + sizeof(Vertex::texPosition) + sizeof(Vertex::lightNormal));
    VAO.linkVBO(5, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color) + sizeof(Vertex::texPosition) + sizeof(Vertex::lightNormal) + sizeof(Vertex::tangent));
    EBO.bind(indices);

    VAO.unbind();
    VBO.unbind();
    EBO.unbind();
}

void Mesh::draw(bool loadTextures) {
    VAO.bind();
    if(movedFlag) {
        movedFlag = false;
        updateTransformation();
    }

    glUniformMatrix4fv(glGetUniformLocation(Shader::getActiveShader(), "model"), 1, GL_FALSE, glm::value_ptr(modelTransformation));
    glUniformMatrix4fv(glGetUniformLocation(Shader::getActiveShader(), "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
    initializeOtherFields();
    if(loadTextures) drawTextures();

    glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, 0);
    VAO.unbind();
}

void Mesh::initializeOtherFields() {}

void Mesh::updateTransformation() {
    rotationMatrix = glm::mat4_cast(getRotation());
    glm::mat4 translationMat = glm::translate(glm::mat4(1), getPosition());
    glm::mat4 scalingMatrix = glm::scale(glm::mat4(1), getScale());

    modelTransformation = translationMat * rotationMatrix * scalingMatrix;
}

Mesh::Mesh(const Mesh &copy) : indices(copy.indices), vertices(copy.vertices), VAO(copy.VAO),
VBO(copy.VBO), EBO(copy.EBO), textureSlot(copy.textureSlot), drawMode(copy.drawMode){}

void Mesh::updateMesh(std::vector<Vertex> *vertices, std::vector<GLuint> *indices) {
    VAO.bind();
    VBO.update(*vertices);
    if(indices) EBO.update(*indices);

    VAO.unbind();
    VBO.unbind();
    EBO.unbind();
}
