#include "SingleTextureMesh.h"

SingleTextureMesh::SingleTextureMesh(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader, std::string diffuseTexture, std::string specularTexture)
: Mesh(vertices, indices, shader) {
    if(!diffuseTexture.empty()) {
        this->diffuseTexture = new Texture(diffuseTexture, textureSlotAllocator++, GL_RGB);
        if(!specularTexture.empty()) this->specularTexture = new Texture(specularTexture, textureSlotAllocator++, GL_RED);
    }
}

void SingleTextureMesh::drawTextures() {
    if(diffuseTexture == nullptr) glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), false);
    else {
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), true);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "diffuseTexture"), diffuseTexture->getSlot());
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "specularTexture"), specularTexture->getSlot());
    }

    diffuseTexture->bind();
    specularTexture->bind();
}

SingleTextureMesh::~SingleTextureMesh() {
    delete diffuseTexture;
    delete specularTexture;
}
