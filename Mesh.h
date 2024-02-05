#ifndef TRINITYENGINE_MESH_H
#define TRINITYENGINE_MESH_H

#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "ElementBufferObject.h"
#include "Shader.h"
#include "MoveableObject.h"
#include "Structs.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <vector>
#include "Camera.h"

class Mesh : public MoveableObject {
protected:
    VertexArrayObject VAO;
    VertexBufferObject VBO;
    ElementBufferObject EBO;
    Shader &shader;

    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    std::vector<Texture> diffuseTextures;
    std::vector<Texture> specularTextures;

    Camera *camera;
    static int textureSlotAllocator;
    int textureSlot;
    float scale = 1.0f;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader,
               std::vector<std::string> diffuseTextures = std::vector<std::string>(), std::vector<std::string> specularTextures = std::vector<std::string>());

    void bind();

    void draw();

    virtual void initializeOtherFields();

    void setCamera(Camera& camera);

    float getScale() const;

    void setScale(float scale);
};

#endif
