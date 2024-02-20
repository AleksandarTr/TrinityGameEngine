#include "SingleTextureMesh.h"
#include "TextureHandler.h"
#include <algorithm>
#include <iostream>

SingleTextureMesh::SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Shader &shader, GLenum drawMode,
                                     TextureInfo diffuseTexture, TextureInfo specularTexture, TextureInfo normalTexture, std::string name)
: Mesh(vertices, indices, shader, drawMode), name(std::move(name)) {
    if(!diffuseTexture.location.empty()) {
        if(diffuseTexture.type == TextureType::None) diffuseTexture.type = TextureType::Diffuse;
        if(specularTexture.type == TextureType::None) specularTexture.type = TextureType::PBR;
        if(normalTexture.type == TextureType::None) normalTexture.type = TextureType::Normal;

        this->diffuseTexture = new Texture(diffuseTexture);
        TextureHandler::getTextureHandler().loadTexture(diffuseTexture, this->diffuseTexture);
        if(!specularTexture.location.empty()) {
            this->specularTexture = new Texture(specularTexture);
            TextureHandler::getTextureHandler().loadTexture(specularTexture, this->specularTexture);
        }
        if(!normalTexture.location.empty()) {
            this->normalTexture = new Texture(normalTexture);
            TextureHandler::getTextureHandler().loadTexture(normalTexture, this->normalTexture);
        }
    }
}

void SingleTextureMesh::drawTextures() {
    if(!diffuseTexture) glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), false);
    else {
        TextureHandler::bindTexture(*diffuseTexture);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), true);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "diffuseTexture"), static_cast<int>(diffuseTexture->getInfo().type));

        if(specularTexture && specularTexture->getId()) {
            TextureHandler::bindTexture(*specularTexture);
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "specularTexture"), static_cast<int>(specularTexture->getInfo().type));
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "applySpecularTexture"), true);
        }
        else {
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "applySpecularTexture"), false);
            glUniform1f(glGetUniformLocation(shader.getProgramID(), "metallic"), metallic);
            glUniform1f(glGetUniformLocation(shader.getProgramID(), "roughness"), roughness);
        }

        if(normalTexture && normalTexture->getId()) {
            TextureHandler::bindTexture(*normalTexture);
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "normalTexture"), static_cast<int>(normalTexture->getInfo().type));
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "applyNormalTexture"), true);
        }
        else glUniform1i(glGetUniformLocation(shader.getProgramID(), "applyNormalTexture"), false);
    }
}

SingleTextureMesh::~SingleTextureMesh() {
    delete diffuseTexture;
    delete specularTexture;
}

SingleTextureMesh::SingleTextureMesh(const SingleTextureMesh &mesh) : Mesh(mesh), name(mesh.name) {
    if(mesh.diffuseTexture) {
        diffuseTexture = new Texture(mesh.diffuseTexture->getInfo());
        TextureHandler::getTextureHandler().loadTexture(mesh.diffuseTexture->getInfo(), diffuseTexture);
    }
    if(mesh.specularTexture) {
        specularTexture = new Texture(mesh.specularTexture->getInfo());
        TextureHandler::getTextureHandler().loadTexture(mesh.specularTexture->getInfo(), specularTexture);
    }
    if(mesh.normalTexture) {
        normalTexture = new Texture(mesh.normalTexture->getInfo());
        TextureHandler::getTextureHandler().loadTexture(mesh.normalTexture->getInfo(), normalTexture);
    }
}

void SingleTextureMesh::setMetallic(float value) {
    metallic = value;
}

void SingleTextureMesh::setRoughness(float value) {
    roughness = value;
}
