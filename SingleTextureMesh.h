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
    TextureInfo occlusionTexture = TextureInfo(), TextureInfo emissionTexture = TextureInfo(), std::string name = ""); //Mesh with only one diffuse texture, specular texture, normal texture, occlusion texture and emission texture

    SingleTextureMesh(const SingleTextureMesh&);

    void drawTextures() override; // Load textures into the shaders
    void initializeOtherFields() override; // Initialize any fields besides the textures

    void setMetallic(float value); //Set the metallic value, used if a PBR texture is not provided
    void setRoughness(float value); //Set the roughness value, used if a PBR texture is not provided
    void setOcclusionStrength(float value); /*Set ambient occlusion strength,
 *which scales occlusion and must be between 0 and 1,
 *0 meaning no occlusion and 1 meaning full occlusion(fully black)*/
    void setNormalScale(float value); //Scale the normals of given in the normal texture
    void setAlphaMode(AlphaMode value); /*Set the mode in which the alpha channel of the textures will be treated:
 *OpaqueTexture means alpha channel is ignored
 *MaskedTexture means alpha channel is not blended, but if it's below the alpha cutoff the texel is discarded
 *BlendedTexture means alpha channel is used to determine the amount of transparency*/
    void setAlphaCutoff(float value); //Cutoff for alpha values of the diffuse texture, below which texels are discarded in MaskedTexture alpha mode
    void setEmissionValues(glm::vec3 values); //Emission values applied to the whole texture, unless emission texture is provided

    AlphaMode getAlphaMode() const; /*Get the current alpha mode, where:
 *OpaqueTexture(0) means alpha channel is ignored
 *MaskedTexture(1) means alpha channel is not blended, but if it's below the alpha cutoff the texel is discarded
 *BlendedTexture(2) means alpha channel is used to determine the amount of transparency*/
    Texture* getTexture(TextureType type); //Get the specified texture based on its type

    ~SingleTextureMesh();
};

#endif
