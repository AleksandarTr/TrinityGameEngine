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
protected:
    VertexArrayObject VAO;
    VertexBufferObject VBO;
    ElementBufferObject EBO;
    Shader &shader;

    std::vector<Vertex> vertices;
    std::vector<Index> indices;

    static int textureSlotAllocator;
    int textureSlot;
    glm::vec3 scale = glm::vec3(1.0f);
    glm::mat4 modelTransformation = glm::mat4(1);
    glm::mat4 rotationMatrix = glm::mat4(1);

public:
    Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader);

    void updateTransformation();

    void bind();

    virtual void drawTextures() = 0;

    void draw();

    virtual void initializeOtherFields();

    glm::vec3 getScale() const;

    void setScale(glm::vec3 scale);
};

#endif
