#include "SingleTextureMesh.h"
#include "TextureHandler.h"
#include <algorithm>
#include <iostream>

SingleTextureMesh::SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, GLenum drawMode,
                                     TextureInfo diffuseTexture, TextureInfo specularTexture, TextureInfo normalTexture, TextureInfo occlusionTexture, std::string name)
: Mesh(vertices, indices, drawMode), name(std::move(name)) {
    if(!diffuseTexture.location.empty()) {
        if (diffuseTexture.type == TextureType::None) diffuseTexture.type = TextureType::Diffuse;
        this->diffuseTexture = new Texture(diffuseTexture);
        TextureHandler::getTextureHandler().loadTexture(diffuseTexture, this->diffuseTexture);
    }

    if(!specularTexture.location.empty()) {
        if (specularTexture.type == TextureType::None) specularTexture.type = TextureType::PBR;
        this->specularTexture = new Texture(specularTexture);
        TextureHandler::getTextureHandler().loadTexture(specularTexture, this->specularTexture);
    }

    if(!normalTexture.location.empty()) {
        if (normalTexture.type == TextureType::None) normalTexture.type = TextureType::Normal;
        this->normalTexture = new Texture(normalTexture);
        TextureHandler::getTextureHandler().loadTexture(normalTexture, this->normalTexture);
    }

    if(!occlusionTexture.location.empty()) {
        if (occlusionTexture.type == TextureType::None) occlusionTexture.type = TextureType::Oclussion;
        this->occlusionTexture = new Texture(occlusionTexture);
        TextureHandler::getTextureHandler().loadTexture(occlusionTexture, this->occlusionTexture);
    }
}

void SingleTextureMesh::drawTextures() {
    if (diffuseTexture && diffuseTexture->getId()) {
        TextureHandler::bindTexture(*diffuseTexture);
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "useTexture"), true);
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "diffuseTexture"),
                    static_cast<int>(diffuseTexture->getInfo().type));
    }
    else glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "useTexture"), false);

    if(specularTexture && specularTexture->getId()) {
        TextureHandler::bindTexture(*specularTexture);
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "specularTexture"), static_cast<int>(specularTexture->getInfo().type));
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applySpecularTexture"), true);
    }
    else {
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applySpecularTexture"), false);
        glUniform1f(glGetUniformLocation(Shader::getActiveShader(), "metallic"), metallic);
        glUniform1f(glGetUniformLocation(Shader::getActiveShader(), "roughness"), roughness);
    }

    if(normalTexture && normalTexture->getId()) {
        TextureHandler::bindTexture(*normalTexture);
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "normalTexture"), static_cast<int>(normalTexture->getInfo().type));
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyNormalTexture"), true);
    }
    else glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyNormalTexture"), false);

    if(occlusionTexture && occlusionTexture->getId()) {
        TextureHandler::bindTexture(*occlusionTexture);
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "occlusionTexture"), static_cast<int>(occlusionTexture->getInfo().type));
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyOcclusionTexture"), true);
    }
    else glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyOcclusionTexture"), false);
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

    if(mesh.occlusionTexture) {
        occlusionTexture = new Texture(mesh.occlusionTexture->getInfo());
        TextureHandler::getTextureHandler().loadTexture(mesh.occlusionTexture->getInfo(), occlusionTexture);
    }
}

void SingleTextureMesh::setMetallic(float value) {
    metallic = value;
}

void SingleTextureMesh::setRoughness(float value) {
    roughness = value;
}
