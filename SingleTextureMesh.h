#ifndef TRINITYENGINE_SINGLETEXTUREMESH_H
#define TRINITYENGINE_SINGLETEXTUREMESH_H

#include "Texture.h"
#include "Mesh.h"

class SingleTextureMesh : public Mesh {
private:
    Texture* diffuseTexture = nullptr;
    Texture* specularTexture = nullptr;

public:
    SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Shader &shader, GLenum drawMode,
    TextureInfo diffuseTexture = TextureInfo(), TextureInfo specularTexture = TextureInfo());

    SingleTextureMesh(const SingleTextureMesh&);

    void drawTextures() override;

    ~SingleTextureMesh();
};

#endif
