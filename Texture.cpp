#include "Texture.h"
void Texture::bind(GLenum target) const {
    glBindTexture(target, textureId);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureId);
}

GLuint Texture::getId() const {
    return textureId;
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextureInfo &Texture::getInfo() {
    return info;
}

Texture::Texture(TextureInfo info) : info(std::move(info)) {}

void Texture::setId(GLuint id) {
    if(!textureId) textureId = id;
}
