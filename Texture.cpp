#include "Texture.h"
void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, textureId);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureId);
}

GLuint Texture::getId() {
    return textureId;
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

const TextureInfo &Texture::getInfo() {
    return info;
}

Texture::Texture(TextureInfo info) : info(std::move(info)) {}
