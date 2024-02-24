#ifndef TRINITYENGINE_TEXTURE_H
#define TRINITYENGINE_TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include <glad/glad.h>
#include "Structs.h"

class Texture {
    friend class TextureHandler;
    friend class Light;

private:
    GLuint textureId = 0;
    TextureInfo info;

public:
    Texture(TextureInfo info);

    void bind();

    ~Texture();

    static void unbind();

    TextureInfo& getInfo();

    GLuint getId();
};

#endif