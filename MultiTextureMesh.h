#ifndef TRINITYENGINE_MULTITEXTUREMESH_H
#define TRINITYENGINE_MULTITEXTUREMESH_H

#include "Mesh.h"

class MultiTextureMesh : public Mesh {
private:
    std::vector<Texture> diffuseTextures = std::vector<Texture>();
    std::vector<Texture> specularTextures = std::vector<Texture>();

public:
    MultiTextureMesh(std::vector<Vertex> &vertices, std::vector<Index> &indices, Shader &shader,
    std::vector<std::string> diffuseTextures = std::vector<std::string>(), std::vector<std::string> specularTextures = std::vector<std::string>());

    void drawTextures() override;
};

#endif
