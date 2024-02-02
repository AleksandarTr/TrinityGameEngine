#include "Object.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Object::Object(GLfloat *vertices, std::size_t vertexCount, std::size_t vertexSize, GLuint * indices, std::size_t indexCount)
: vertices(vertices), indices(indices), vertexSize(vertexSize), vertexCount(vertexCount), indexCount(indexCount) {}

void Object::bind() {
    VAO.bind();
    VBO.bind(vertices, vertexCount * vertexSize * sizeof(GLfloat));
    VAO.linkVBO(0, 3, GL_FLOAT, vertexSize * sizeof(float), 0);
    VAO.linkVBO(1, 4, GL_FLOAT, vertexSize * sizeof(float), 3 * sizeof(float));
    VAO.linkVBO(2, 2, GL_FLOAT, vertexSize * sizeof(float), 7 * sizeof(float));
    EBO.bind(indices, indexCount * sizeof(GLuint));

    //Unbind VBO AND VAO
    VAO.unbind();
    VBO.unbind();
    EBO.unbind();
}

void Object::draw() {
    VAO.bind();

    glDrawElements(GL_TRIANGLES, indexCount * 3, GL_UNSIGNED_INT, 0);

    VAO.unbind();
}

void Object::rotate(float angle, glm::vec3 normal, Shader &shader, const char *uniform) {
    glm::mat4 rotationMat(1);
    rotationMat = glm::rotate(rotationMat, angle, normal);
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), uniform), 1, GL_FALSE, glm::value_ptr(rotationMat));
}
