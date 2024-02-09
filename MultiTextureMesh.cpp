#include "MultiTextureMesh.h"

MultiTextureMesh::MultiTextureMesh(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader, std::vector<std::string> diffuseTextures, std::vector<std::string> specularTextures)
: Mesh(std::move(vertices), std::move(indices), shader, GL_TRIANGLES) {
    textureSlot = textureSlotAllocator;

    while(specularTextures.size() > diffuseTextures.size()) specularTextures.pop_back();

    int i = 0;
    while(i < diffuseTextures.size()){
        bool found = false;
        for(int j = 0; j < this->vertices.size(); j++)
            if((int) this->vertices[j].texPosition.z == i) {
                found = true;
                break;
            }

        if(!found) {
            if(i < specularTextures.size()) specularTextures.erase(specularTextures.begin() + i);

            for(auto & vertex : this->vertices)
                if((int) vertex.texPosition.z > i) vertex.texPosition.z--;
            diffuseTextures.erase(diffuseTextures.begin() + i);
        }
        else i++;
    }

    if(diffuseTextures.size() > 16) throw std::invalid_argument("More than 16 textures cannot be applied to a single object.");

    textureSlotAllocator += diffuseTextures.size() + specularTextures.size();

    for(int i = 0; i < diffuseTextures.size(); i++)
        this->diffuseTextures.emplace_back(diffuseTextures[i], textureSlot + i, GL_RGB);
    for(int i = 0; i < diffuseTextures.size(); i++)
        this->specularTextures.emplace_back(specularTextures[i], textureSlot + diffuseTextures.size() + i, GL_RED);
}

void MultiTextureMesh::drawTextures() {
    if(diffuseTextures.empty()) glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), false);
    else {
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "useTexture"), true);
        for(int i = 0; i < 16; i++) {
            char uniformName[25];
            sprintf(uniformName, "diffuseTextures[%d]", i);
            glUniform1i(glGetUniformLocation(shader.getProgramID(), uniformName), diffuseTextures[i].getSlot());
        }
        for(int i = 0; i < 16; i++) {
            char specularName[25];
            sprintf(specularName, "specularTextures[%d]", i);

            glUniform1i(glGetUniformLocation(shader.getProgramID(), specularName), specularTextures[i].getSlot());
        }
    }

    for(Texture &texture : diffuseTextures) texture.bind();
    for(Texture &texture : specularTextures) texture.bind();
}
