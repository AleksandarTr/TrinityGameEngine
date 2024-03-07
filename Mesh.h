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
#include "View.h"

class Mesh : public Movable {
    friend class Model;

protected:
    std::vector<Vertex> &vertices;
    std::vector<GLuint> &indices;
    GLenum drawMode = GL_TRIANGLES;
    bool doubleSided = false;

    //Mesh buffers
    VertexArrayObject VAO = VertexArrayObject();
    VertexBufferObject VBO = VertexBufferObject();
    ElementBufferObject EBO = ElementBufferObject();

    glm::mat4 modelTransformation = glm::mat4(1);
    glm::mat4 rotationMatrix = glm::mat4(1); //Needed to adjust the normal vectors

    float boundingSphere; //Outermost point of the mesh, updated based on scale
    void calculateBoundingSphere();
public:
    Mesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, GLenum drawMode);
    Mesh(const Mesh&);

    void updateTransformation(); // Update transformation matrices sent to the shaders

    void bind(); // Bind vertices and indices to the buffers, so opengl can display them

    virtual void drawTextures() = 0; // Load textures if there are any

    void draw(bool loadTextures = true); // Render the mesh, if loadTexture is disabled, only the shape of the mesh will render

    virtual void initializeOtherFields(); // Initialize any fields which a derived class might have

    void updateMesh(std::vector<Vertex> *vertices, std::vector<GLuint> *indices = nullptr, bool newBuffers = false); /*Update vertices and optionally indices,
    if newBuffers is enabled, new buffers will be created to store vertices and indices, which is usually used if the current buffers cannot store new data,
    otherwise the same buffers will be used
    */

    void scale(glm::vec3 scaling) override; // Apply a scaling to the mesh and update the bounding sphere

    void setScale(glm::vec3 scale) override; //Set the scale of the mesh and update the bounding sphere

    bool isVisible(); //Check if the mesh is within the frustum of the currently active view

    void setDoubleSided(bool value); //Sets if both of the sides of the mesh should be rendered, by default its false
};

#endif
