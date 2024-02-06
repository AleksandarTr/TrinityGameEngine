#ifndef TRINITYENGINE_SINGLETEXTUREMESH_H
#define TRINITYENGINE_SINGLETEXTUREMESH_H

#include "Texture.h"
#include "Mesh.h"

class SingleTextureMesh : public Mesh {
private:
    Texture* diffuseTexture = nullptr;
    Texture* specularTexture = nullptr;

public:
    SingleTextureMesh(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader,
    std::string diffuseTexture = "", std::string specularTexture = "");

    void drawTextures() override;

    ~SingleTextureMesh();
};

#endif
