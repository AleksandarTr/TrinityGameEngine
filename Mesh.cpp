#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

int Mesh::textureSlotAllocator = 0;

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader, std::vector<std::string> diffuseTextures, std::vector<std::string> specularTextures)
: vertices(std::move(vertices)), indices(std::move(indices)), shader(shader) {

    textureSlot = textureSlotAllocator;
    textureSlotAllocator += diffuseTextures.size() + specularTextures.size();

    for(int i = 0; i < diffuseTextures.size(); i++)
        this->diffuseTextures.emplace_back(diffuseTextures[i], textureSlot + i, GL_RGB);
    for(int i = 0; i < diffuseTextures.size(); i++)
        this->specularTextures.emplace_back(specularTextures[i], textureSlot + diffuseTextures.size() + i, GL_RED);
}

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
    glm::vec3 xAxis(1.0f, 0, 0);
    glm::vec3 yAxis(0, 1.0f, 0);
    glm::vec3 zAxis(0, 0, 1.0f);

    glm::mat4 rotationMat(1);
    rotationMat = glm::rotate(rotationMat, getRotation().x, xAxis);
    rotationMat = glm::rotate(rotationMat, getRotation().y, yAxis);
    rotationMat = glm::rotate(rotationMat, getRotation().z, zAxis);
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMat));

    glm::mat4 translationMat(1);
    translationMat = glm::translate(translationMat, getPosition());
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "camMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getCameraMat()));
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "position"), 1, GL_FALSE, glm::value_ptr(translationMat));
    glUniform1f(glGetUniformLocation(shader.getProgramID(), "scale"), 3.0f);
    if(diffuseTextures.empty()) glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), false);
    else {
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), true);
        for(int i = 0; i < 16; i++) {
            char uniformName[25];
            sprintf(uniformName, "diffuseTextures[%d]", i);
            glUniform1i(glGetUniformLocation(shader.getProgramID(), uniformName), diffuseTextures[i].getSlot());
        }
        for(int i = 0; i < 16; i++) {
            char specularName[25];
            sprintf(specularName, "specularTextures[%d]", i);

            glUniform1i(glGetUniformLocation(shader.getProgramID(), specularName), specularTextures[i].getSlot());
        }
    }
    initializeOtherFields();

    for(Texture &texture : diffuseTextures) texture.bind();
    for(Texture &texture : specularTextures) texture.bind();

    glDrawElements(GL_TRIANGLES, indices.size() * sizeof(Index) / sizeof(GLfloat), GL_UNSIGNED_INT, 0);
    Texture::unbind();
    VAO.unbind();
}

void Mesh::setCamera(Camera &camera) {
    this->camera = &camera;
}

void Mesh::initializeOtherFields() {}
