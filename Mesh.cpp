#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Mesh::Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, GLenum drawMode)
: vertices(vertices), indices(indices), drawMode(drawMode) {}

void Mesh::bind() {
    VAO.bind();
    VBO.bind(vertices);
    //Bind vertex positions
    VAO.linkVBO(0, 3, GL_FLOAT, sizeof(Vertex), 0);
    //Bind vertex color
    VAO.linkVBO(1, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position));
    //Bind vertex texture coordinates
    VAO.linkVBO(2, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color));
    //Bind vertex normals
    VAO.linkVBO(3, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color) + sizeof(Vertex::texPosition));
    //Bind vertex tangents
    VAO.linkVBO(4, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color) + sizeof(Vertex::texPosition) + sizeof(Vertex::lightNormal));
    //Bind vertex bitangents
    VAO.linkVBO(5, 3, GL_FLOAT, sizeof(Vertex), sizeof(Vertex::position) + sizeof(Vertex::color) + sizeof(Vertex::texPosition) + sizeof(Vertex::lightNormal) + sizeof(Vertex::tangent));
    EBO.bind(indices);

    VAO.unbind();
    VBO.unbind();
    EBO.unbind();
    calculateBoundingSphere();
}

void Mesh::draw(bool loadTextures) {
    //Don't draw mesh if it's not visible
    if(!isVisible()) return;

    VAO.bind();
    //Update mesh only when it changed
    if(movedFlag) {
        movedFlag = false;
        updateTransformation();
    }

    //Mesh loads only mesh transformation matrices
    glUniformMatrix4fv(glGetUniformLocation(Shader::getActiveShader(), "model"), 1, GL_FALSE, glm::value_ptr(modelTransformation));
    glUniformMatrix4fv(glGetUniformLocation(Shader::getActiveShader(), "rotation"), 1, GL_FALSE, glm::value_ptr(rotationMatrix));
    if(loadTextures) {
        //All other fields are loaded by derived classes
        initializeOtherFields();
        drawTextures();
        if(doubleSided) glDisable(GL_CULL_FACE);
        else glEnable(GL_CULL_FACE);
    }

    glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, 0);
    VAO.unbind();
}

void Mesh::initializeOtherFields() {}

void Mesh::updateTransformation() {
    //Update rotation matrix for rotating vertex normals
    rotationMatrix = glm::mat4_cast(getRotation());
    glm::mat4 translationMat = glm::translate(glm::mat4(1), getPosition());
    glm::mat4 scalingMatrix = glm::scale(glm::mat4(1), getScale());

    //Get complete model transformation
    modelTransformation = translationMat * rotationMatrix * scalingMatrix;
}

Mesh::Mesh(const Mesh &copy) : indices(copy.indices), vertices(copy.vertices), VAO(copy.VAO),
VBO(copy.VBO), EBO(copy.EBO), drawMode(copy.drawMode){}

void Mesh::updateMesh(std::vector<Vertex> *vertices, std::vector<GLuint> *indices, bool newBuffers) {
    VAO.bind();
    if(newBuffers) {
        //Make new buffer for vertices, used for when there are more vertices than before,
        //otherwise newBuffers should be false
        VBO.bind(*vertices, true);
        if (indices) EBO.bind(*indices, true);
    }
    else {
        VBO.update(*vertices);
        if (indices) EBO.update(*indices);
    }

    VAO.unbind();
    VBO.unbind();
    EBO.unbind();

    //Update vertices and indices references
    *(reinterpret_cast<void**>(&drawMode) - 2) = reinterpret_cast<void*>(vertices);
    if(indices) *(reinterpret_cast<void**>(&drawMode) - 1) = reinterpret_cast<void*>(indices);

    calculateBoundingSphere(); //Recalculate bounding sphere
}

void Mesh::calculateBoundingSphere() {
    //Finding the farthest coordinate from the center (position (0, 0, 0))
    boundingSphere = 0;
    for(auto vertex : vertices) {
        boundingSphere = std::max(boundingSphere, std::abs(vertex.position.x));
        boundingSphere = std::max(boundingSphere, std::abs(vertex.position.y));
        boundingSphere = std::max(boundingSphere, std::abs(vertex.position.z));
    }

    boundingSphere *= std::max(std::max(getScale().x, getScale().y), getScale().z);
}

void Mesh::setScale(glm::vec3 scale) {
    //BoundingSphere is scaled based on the most extruded scale
    float prevScale = std::max(std::max(getScale().x, getScale().y), getScale().z);
    float newScale = std::max(std::max(scale.x, scale.y), scale.z);
    boundingSphere *= newScale / prevScale;

    Movable::setScale(scale);
}

void Mesh::scale(glm::vec3 scaling) {
    //BoundingSphere is scaled based on the most extruded scale
    float prevScale = std::max(std::max(getScale().x, getScale().y), getScale().z);
    Movable::scale(scaling);
    float newScale = std::max(std::max(getScale().x, getScale().y), getScale().z);
    boundingSphere *= newScale / prevScale;
}

bool Mesh::isVisible() {
    //For each side of the frustum, it's checked if at least some part of
    // the bounding sphere is on the side of the plane is on the side the normal is pointing towards
    const View::Plane *cameraFrustum = View::getActiveView()->getViewFrustum();
    for(int i = 0; i < 6; i++) {
        if(glm::dot(cameraFrustum->normal, getPosition()) + cameraFrustum->d < -boundingSphere) return false;
        cameraFrustum++;
    }
    return true;
}

void Mesh::setDoubleSided(bool value) {
    doubleSided = true;
}
