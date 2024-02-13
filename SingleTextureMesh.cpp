#include "SingleTextureMesh.h"
#include <algorithm>

SingleTextureMesh::SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Shader &shader, GLenum drawMode, std::string diffuseTexture, std::string specularTexture)
: Mesh(vertices, indices, shader, drawMode) {
    if(!diffuseTexture.empty()) {

        auto itr = loadedTextures.begin();
        for(; itr != loadedTextures.end(); itr++)
            if((*itr)->getLocation() == diffuseTexture) break;
        if(itr != loadedTextures.end()) this->diffuseTexture = *itr;
        else this->diffuseTexture = new Texture(diffuseTexture, textureSlotAllocator++, GL_RGB);

        if(!specularTexture.empty()) {
            itr = loadedTextures.begin();
            for (; itr != loadedTextures.end(); itr++)
                if ((*itr)->getLocation() == specularTexture) break;
            if (itr != loadedTextures.end()) this->specularTexture = *itr;
            else this->specularTexture = new Texture(specularTexture, textureSlotAllocator++, GL_RED);
        }
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
            specularTexture->bind();
        }
    }
}

SingleTextureMesh::~SingleTextureMesh() {
    delete diffuseTexture;
    delete specularTexture;
}

SingleTextureMesh::SingleTextureMesh(const SingleTextureMesh &mesh) : Mesh(mesh), diffuseTexture(mesh.diffuseTexture),
specularTexture(mesh.specularTexture) {}
