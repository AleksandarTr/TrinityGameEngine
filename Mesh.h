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
public:
    VertexArrayObject VAO = VertexArrayObject();
    VertexBufferObject VBO = VertexBufferObject();
    ElementBufferObject EBO = ElementBufferObject();
    Shader &shader;

    std::vector<Vertex> vertices = std::vector<Vertex>();
    std::vector<Index> indices = std::vector<Index>();
    GLenum drawMode = GL_TRIANGLES;

    static int textureSlotAllocator;
    int textureSlot = -1;
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 modelTransformation = glm::mat4(1);
    glm::mat4 rotationMatrix = glm::mat4(1);

public:
    Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader, GLenum drawMode);

    void updateTransformation();

    void bind();

    virtual void drawTextures() = 0;

    void draw();

    virtual void initializeOtherFields();

    glm::vec3 getScale() const;

    void setScale(glm::vec3 scale);

    void setTransformMatrix(glm::mat4 transform);
};

#endif
