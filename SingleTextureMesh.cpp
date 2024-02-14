#include "SingleTextureMesh.h"
#include <algorithm>

SingleTextureMesh::SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Shader &shader, GLenum drawMode,
                                     TextureInfo diffuseTexture, TextureInfo specularTexture, TextureInfo normalTexture)
: Mesh(vertices, indices, shader, drawMode) {
    if(!diffuseTexture.location.empty()) {
        this->diffuseTexture = loadTexture(diffuseTexture);
        if(!specularTexture.location.empty()) this->specularTexture = loadTexture(specularTexture);
        if(!normalTexture.location.empty()) this->normalTexture = loadTexture(normalTexture);
    }
}

void SingleTextureMesh::drawTextures() {
    if(diffuseTexture == nullptr) glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), false);
    else {
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), true);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "diffuseTexture"), diffuseTexture->getSlot());
        diffuseTexture->bind();

        if(specularTexture) {
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "specularTexture"), specularTexture->getSlot());
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "applySpecularTexture"), true);
            specularTexture->bind();
        }
        else glUniform1i(glGetUniformLocation(shader.getProgramID(), "applySpecularTexture"), false);

        if(normalTexture) {
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "normalTexture"), normalTexture->getSlot());
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "applyNormalTexture"), true);
            normalTexture->bind();
        }
        else glUniform1i(glGetUniformLocation(shader.getProgramID(), "applyNormalTexture"), false);
    }
}

SingleTextureMesh::~SingleTextureMesh() {
    delete diffuseTexture;
    delete specularTexture;
}

SingleTextureMesh::SingleTextureMesh(const SingleTextureMesh &mesh) : Mesh(mesh), diffuseTexture(mesh.diffuseTexture),
specularTexture(mesh.specularTexture), normalTexture(mesh.normalTexture) {}

Texture *SingleTextureMesh::loadTexture(TextureInfo texture) {
    auto itr = loadedTextures.begin();
    for (; itr != loadedTextures.end(); itr++)
        if ((*itr)->getLocation() == texture.location) break;
    if (itr != loadedTextures.end()) return *itr;
    else return new Texture(texture, textureSlotAllocator++);
}
