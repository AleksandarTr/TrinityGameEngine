#ifndef TRINITYENGINE_TEXTURE_H
#define TRINITYENGINE_TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>

class Texture {
private:
    GLuint textureId;

public:
    Texture(std::string location);

    void bind();

    ~Texture();
};

#endif