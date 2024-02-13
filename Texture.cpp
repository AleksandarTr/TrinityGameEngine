#include "Texture.h"

Texture::Texture(TextureInfo info, GLuint slot) : slot(slot), location(info.location) {
    int imgW, imgH, colChNum;
    stbi_set_flip_vertically_on_load(true);
    unsigned  char* bytes = stbi_load(location.c_str(), &imgW, &imgH, &colChNum, 0);

    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, info.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, info.magFilter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, info.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, info.wrapT);

    int format;
    if(info.format == "image/png") format = GL_RGBA;
    else if(info.format == "image/jpeg") format = GL_RGB;
    else format = GL_RED;

    glTexImage2D(GL_TEXTURE_2D, 0, format, imgW, imgH, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
    bind();
}

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

GLuint Texture::getSlot() {
    return slot;
}

std::string Texture::getLocation() {
    return location;
}
