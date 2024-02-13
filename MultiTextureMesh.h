#ifndef TRINITYENGINE_MULTITEXTUREMESH_H
#define TRINITYENGINE_MULTITEXTUREMESH_H

#include "Mesh.h"

class MultiTextureMesh : public Mesh {
private:
    std::vector<Texture> diffuseTextures = std::vector<Texture>();
    std::vector<Texture> specularTextures = std::vector<Texture>();

public:
    MultiTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Shader &shader,
    std::vector<TextureInfo> diffuseTextures = std::vector<TextureInfo>(), std::vector<TextureInfo> specularTextures = std::vector<TextureInfo>());

    void drawTextures() override;
};

#endif
