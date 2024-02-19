#include "SingleTextureMesh.h"
#include "TextureHandler.h"
#include <algorithm>

SingleTextureMesh::SingleTextureMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, Shader &shader, GLenum drawMode,
                                     TextureInfo diffuseTexture, TextureInfo specularTexture, TextureInfo normalTexture)
: Mesh(vertices, indices, shader, drawMode) {
    if(!diffuseTexture.location.empty()) {
        diffuseTexture.type = TextureType::Diffuse;
        specularTexture.type = TextureType::PBR;
        normalTexture.type = TextureType::Normal;
        TextureHandler::getTextureHandler().loadTexture(diffuseTexture, &(this->diffuseTexture));
        if(!specularTexture.location.empty())
            TextureHandler::getTextureHandler().loadTexture(specularTexture, &(this->specularTexture));
        if(!normalTexture.location.empty())
            TextureHandler::getTextureHandler().loadTexture(normalTexture, &(this->normalTexture));
    }
}

void SingleTextureMesh::drawTextures() {
    if(diffuseTexture == nullptr) glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), false);
    else {
        glActiveTexture(GL_TEXTURE0 + static_cast<int>(TextureType::Diffuse));
        diffuseTexture->bind();
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), true);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "diffuseTexture"), GL_TEXTURE0 + static_cast<int>(TextureType::Diffuse));

        if(specularTexture) {
            glActiveTexture(GL_TEXTURE0 + static_cast<int>(TextureType::PBR));
            specularTexture->bind();
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "specularTexture"), GL_TEXTURE0 + static_cast<int>(TextureType::PBR));
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "applySpecularTexture"), true);
        }
        else {
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "applySpecularTexture"), false);
            glUniform1f(glGetUniformLocation(shader.getProgramID(), "metallic"), metallic);
            glUniform1f(glGetUniformLocation(shader.getProgramID(), "roughness"), roughness);
        }

        if(normalTexture) {
            glActiveTexture(GL_TEXTURE0 + static_cast<int>(TextureType::Normal));
            normalTexture->bind();
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "normalTexture"), GL_TEXTURE0 + static_cast<int>(TextureType::Normal));
            glUniform1i(glGetUniformLocation(shader.getProgramID(), "applyNormalTexture"), true);
        }
        else glUniform1i(glGetUniformLocation(shader.getProgramID(), "applyNormalTexture"), false);
    }
}

SingleTextureMesh::~SingleTextureMesh() {
    delete diffuseTexture;
    delete specularTexture;
}

SingleTextureMesh::SingleTextureMesh(const SingleTextureMesh &mesh) : Mesh(mesh) {
    if(mesh.diffuseTexture) TextureHandler::getTextureHandler().loadTexture(mesh.diffuseTexture->getInfo(), &diffuseTexture);
    if(mesh.specularTexture) TextureHandler::getTextureHandler().loadTexture(mesh.specularTexture->getInfo(), &specularTexture);
    if(mesh.normalTexture) TextureHandler::getTextureHandler().loadTexture(mesh.normalTexture->getInfo(), &normalTexture);
}

void SingleTextureMesh::setMetallic(float value) {
    metallic = value;
}

void SingleTextureMesh::setRoughness(float value) {
    roughness = value;
}
