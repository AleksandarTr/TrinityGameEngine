#include "BaseObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

BaseObject::BaseObject(GLfloat *vertices, std::size_t vertexCount, std::size_t vertexSize, GLuint * indices, std::size_t indexCount, Shader &shader)
: vertices(vertices), indices(indices), vertexCount(vertexCount), vertexSize(vertexSize), indexCount(indexCount), shader(shader) {
}

void BaseObject::bind() {
    VAO.bind();
    VBO.bind(vertices, vertexCount * vertexSize * sizeof(GLfloat));
    VAOlinkVBO();
    EBO.bind(indices, indexCount * 3 * sizeof(GLuint));

    //Unbind VBO AND VAO
    VAO.unbind();
    VBO.unbind();
    EBO.unbind();
}

void BaseObject::draw() {
    shader.activate();
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
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), "position"), 1, GL_FALSE, glm::value_ptr(translationMat));

    VAO.bind();
    glDrawElements(GL_TRIANGLES, indexCount * 3, GL_UNSIGNED_INT, 0);
    VAO.unbind();
}
