#ifndef TRINITYENGINE_SINGLETEXTUREMESH_H
#define TRINITYENGINE_SINGLETEXTUREMESH_H

#include "Texture.h"
#include "Mesh.h"

class SingleTextureMesh : public Mesh {
private:
    Texture* diffuseTexture = nullptr;
    Texture* specularTexture = nullptr;
    Texture* normalTexture = nullptr;
    Texture* occlusionTexture = nullptr;

    float metallic = 0;
    float roughness = 0;
    std::string name;

public:
    SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, GLenum drawMode,
    TextureInfo diffuseTexture = TextureInfo(), TextureInfo specularTexture = TextureInfo(), TextureInfo normalTexture = TextureInfo(),
    TextureInfo occlusionTexture = TextureInfo(), std::string name = "");

    SingleTextureMesh(const SingleTextureMesh&);

    void drawTextures() override;

    ~SingleTextureMesh();

    void setMetallic(float value);

    void setRoughness(float value);

    Texture* getTexture(TextureType type);
};

#endif
