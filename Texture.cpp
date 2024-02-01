#include "Texture.h"

Texture::Texture(std::string location) {
    int imgW, imgH, colChNum;
    stbi_set_flip_vertically_on_load(true);
    unsigned  char* bytes = stbi_load(location.c_str(), &imgW, &imgH, &colChNum, 0);

    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgW, imgH, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, textureId);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureId);
}
