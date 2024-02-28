#ifndef TRINITYENGINE_MESH_H
#define TRINITYENGINE_MESH_H

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include "Movable.h"
#include "Structs.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include "Camera.h"

class Mesh : public Movable {
    friend class Model;

protected:
    VertexArrayObject VAO = VertexArrayObject();
    VertexBufferObject VBO = VertexBufferObject();
    ElementBufferObject EBO = ElementBufferObject();

    std::vector<Vertex> &vertices;
    std::vector<GLuint> &indices;
    GLenum drawMode = GL_TRIANGLES;

    glm::mat4 modelTransformation = glm::mat4(1);
    glm::mat4 rotationMatrix = glm::mat4(1);

    float boundingSphere;
    void calculateBoundingSphere();

    int textureSlot = -1;
public:
    Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, GLenum drawMode);

    Mesh(const Mesh&);

    void updateTransformation();

    void bind();

    virtual void drawTextures() = 0;

    void draw(bool loadTextures = true);

    virtual void initializeOtherFields();

    void updateMesh(std::vector<Vertex> *vertices, std::vector<GLuint> *indices = nullptr);

    void scale(glm::vec3 scaling) override;

    void setScale(glm::vec3 scale) override;

    bool isVisible();
};

#endif
