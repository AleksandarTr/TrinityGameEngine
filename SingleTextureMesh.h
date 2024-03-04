#ifndef TRINITYENGINE_SINGLETEXTUREMESH_H
#define TRINITYENGINE_SINGLETEXTUREMESH_H

#include "Texture.h"
#include "Mesh.h"

class SingleTextureMesh : public Mesh {
public:
    enum AlphaMode {
        OpaqueTexture, MaskedTexture, BlendedTexture
    };

private:
    Texture* diffuseTexture = nullptr;
    Texture* specularTexture = nullptr;
    Texture* normalTexture = nullptr;
    Texture* occlusionTexture = nullptr;
    Texture* emissionTexture = nullptr;

    float metallic = 0;
    float roughness = 0;
    float occlusionStrength = 1.0f;
    float normalScale = 1.0f;
    glm::vec3 emissionValues = glm::vec3(0);

    float alphaCutoff = 0;
    AlphaMode alphaMode = OpaqueTexture;
    std::string name;

public:
    SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, GLenum drawMode,
    TextureInfo diffuseTexture = TextureInfo(), TextureInfo specularTexture = TextureInfo(), TextureInfo normalTexture = TextureInfo(),
    TextureInfo occlusionTexture = TextureInfo(), TextureInfo emissionTexture = TextureInfo(), std::string name = "");

    SingleTextureMesh(const SingleTextureMesh&);

    void drawTextures() override;
    void initializeOtherFields() override;

    void setMetallic(float value);
    void setRoughness(float value);
    void setOcclusionStrength(float value);
    void setNormalScale(float value);
    void setAlphaMode(AlphaMode value);
    void setAlphaCutoff(float value);
    void setEmissionValues(glm::vec3 values);

    AlphaMode getAlphaMode() const;
    Texture* getTexture(TextureType type);

    ~SingleTextureMesh();
};

#endif
