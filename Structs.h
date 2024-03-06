#ifndef TRINITYENGINE_STRUCTS_H
#define TRINITYENGINE_STRUCTS_H

#include <glm/glm.hpp>
#include <string>

struct Vertex {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 color = glm::vec3(0);
    glm::vec3 texPosition = glm::vec3(0);
    glm::vec3 lightNormal = glm::vec3(1);
    glm::vec3 tangent = glm::vec3(1);
    glm::vec3 bitangent = glm::vec3(1);
};

enum LightingType {PointLight, DirectionalLight, SpotLight};

enum TextureType {NoTextureType = -1, TextTexture, DiffuseTexture, PBRTexture, NormalTextre, EmissiveTexture, OcclusionTexture,
        TE_ShadowMap0, TE_ShadowMap1, TE_ShadowMap2, TE_ShadowMap3, TE_ShadowMap4, TE_ShadowMap5, TE_ShadowMap6, TE_ShadowMap7,
    TE_ShadowMap8, TE_ShadowMap9, TE_ShadowMap10, TE_ShadowMap11, TE_ShadowMap12, TE_ShadowMap13, TE_ShadowMap14, TE_ShadowMap15, TE_TextureTypeCount};

struct TextureInfo {
    std::string location = ""; //Location of the file, which includes its extension
    std::string format = "image/jpeg"; //Format of the image (there is support for jpegs and pngs)
    //OpenGL texture parameters
    int magFilter = GL_NEAREST;
    int minFilter = GL_NEAREST;
    int wrapS = GL_REPEAT;
    int wrapT = GL_REPEAT;
    int width, height; //width and height of the image file
    TextureType type = NoTextureType; //Number which decides which slot the image will be bound to
};

#endif
