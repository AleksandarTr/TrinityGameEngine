#include "SingleTextureMesh.h"
#include "TextureHandler.h"
#include <algorithm>
#include <iostream>

SingleTextureMesh::SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, GLenum drawMode,
                                     TextureInfo diffuseTexture, TextureInfo specularTexture, TextureInfo normalTexture,
                                     TextureInfo occlusionTexture, TextureInfo emissionTexture, std::string name)
: Mesh(vertices, indices, drawMode), name(std::move(name)) {
    if(!diffuseTexture.location.empty()) {
        if (diffuseTexture.type == NoTextureType) diffuseTexture.type = DiffuseTexture;
        this->diffuseTexture = new Texture(diffuseTexture);
        TextureHandler::loadTexture(diffuseTexture, this->diffuseTexture);
    }

    if(!specularTexture.location.empty()) {
        if (specularTexture.type == NoTextureType) specularTexture.type = PBRTexture;
        this->specularTexture = new Texture(specularTexture);
        TextureHandler::loadTexture(specularTexture, this->specularTexture);
    }

    if(!normalTexture.location.empty()) {
        if (normalTexture.type == NoTextureType) normalTexture.type = NormalTextre;
        this->normalTexture = new Texture(normalTexture);
        TextureHandler::loadTexture(normalTexture, this->normalTexture);
    }

    if(!occlusionTexture.location.empty()) {
        if (occlusionTexture.type == NoTextureType) occlusionTexture.type = OcclusionTexture;
        this->occlusionTexture = new Texture(occlusionTexture);
        TextureHandler::loadTexture(occlusionTexture, this->occlusionTexture);
    }

    if(!emissionTexture.location.empty()) {
        if (emissionTexture.type == NoTextureType) emissionTexture.type = EmissiveTexture;
        this->emissionTexture = new Texture(emissionTexture);
        TextureHandler::loadTexture(emissionTexture, this->emissionTexture);
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

    if(emissionTexture && emissionTexture->getId()) {
        TextureHandler::bindTexture(*emissionTexture);
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "emissionTexture"), static_cast<int>(emissionTexture->getInfo().type));
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyEmissionTexture"), true);
    }
    else {
        glUniform1i(glGetUniformLocation(Shader::getActiveShader(), "applyEmissionTexture"), false);
        glUniform3f(glGetUniformLocation(Shader::getActiveShader(), "emissionValues"), emissionValues.r, emissionValues.g, emissionValues.b);
    }

    glUniform1f(glGetUniformLocation(Shader::getActiveShader(), "occlusionStrength"), occlusionStrength);
    glUniform1f(glGetUniformLocation(Shader::getActiveShader(), "normalScale"), normalScale);
    glUniform1f(glGetUniformLocation(Shader::getActiveShader(), "alphaCutoff"), alphaCutoff);
}

SingleTextureMesh::~SingleTextureMesh() {
    delete diffuseTexture;
    delete specularTexture;
}

SingleTextureMesh::SingleTextureMesh(const SingleTextureMesh &mesh) : Mesh(mesh), name(mesh.name) {
    if(mesh.diffuseTexture) {
        diffuseTexture = new Texture(mesh.diffuseTexture->getInfo());
        TextureHandler::loadTexture(mesh.diffuseTexture->getInfo(), diffuseTexture);
    }

    if(mesh.specularTexture) {
        specularTexture = new Texture(mesh.specularTexture->getInfo());
        TextureHandler::loadTexture(mesh.specularTexture->getInfo(), specularTexture);
    }

    if(mesh.normalTexture) {
        normalTexture = new Texture(mesh.normalTexture->getInfo());
        TextureHandler::loadTexture(mesh.normalTexture->getInfo(), normalTexture);
    }

    if(mesh.occlusionTexture) {
        occlusionTexture = new Texture(mesh.occlusionTexture->getInfo());
        TextureHandler::loadTexture(mesh.occlusionTexture->getInfo(), occlusionTexture);
    }
}

void SingleTextureMesh::setMetallic(float value) {
    metallic = value;
}

void SingleTextureMesh::setRoughness(float value) {
    roughness = value;
}

Texture *SingleTextureMesh::getTexture(TextureType type) {
    switch(type){
        case DiffuseTexture:
            return diffuseTexture;
        case PBRTexture:
            return specularTexture;
        case NormalTextre:
            return normalTexture;
        case OcclusionTexture:
            return occlusionTexture;
        case EmissiveTexture:
            return emissionTexture;
        default:
            throw std::out_of_range("There is no texture with given type");
    }
}

void SingleTextureMesh::setOcclusionStrength(float value) {
    if(value < 0 || value > 1) throw std::invalid_argument("Occlusion strength must be between 0 and 1");
    occlusionStrength = value;
}

void SingleTextureMesh::setNormalScale(float value) {
    normalScale = value;
}

void SingleTextureMesh::setAlphaCutoff(float value) {
    if(alphaMode != MaskedTexture) return;
    alphaCutoff = value;
}

void SingleTextureMesh::setAlphaMode(SingleTextureMesh::AlphaMode value) {
    if(value < OpaqueTexture || value > BlendedTexture) throw std::invalid_argument("Invalid alpha mode given");
    alphaMode = value;
    if(alphaMode != MaskedTexture) alphaCutoff = 0;
}

SingleTextureMesh::AlphaMode SingleTextureMesh::getAlphaMode() const {
    return alphaMode;
}

void SingleTextureMesh::initializeOtherFields() {
    if(alphaMode == BlendedTexture) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
}

void SingleTextureMesh::setEmissionValues(glm::vec3 values) {
    emissionValues = values;
}
