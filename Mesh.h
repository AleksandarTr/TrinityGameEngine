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
    VertexArrayObject VAO = VertexArrayObject();
    VertexBufferObject VBO = VertexBufferObject();
    ElementBufferObject EBO = ElementBufferObject();
    Shader &shader;

    std::vector<Vertex> vertices = std::vector<Vertex>();
    std::vector<GLuint> indices = std::vector<GLuint>();
    GLenum drawMode = GL_TRIANGLES;

    glm::mat4 modelTransformation = glm::mat4(1);
    glm::mat4 rotationMatrix = glm::mat4(1);

    int textureSlot = -1;
    static int textureSlotAllocator;
    static std::vector<Texture*> loadedTextures;

public:
    Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Shader &shader, GLenum drawMode);

    Mesh(const Mesh&);

    void updateTransformation();

    void bind();

    virtual void drawTextures() = 0;

    void draw();

    virtual void initializeOtherFields();

    static int getTextureSlot();
};

#endif
