#ifndef TRINITYENGINE_TEXTURE_H
#define TRINITYENGINE_TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include <glad/glad.h>
#include "Structs.h"

class Texture {
private:
    GLuint textureId = 0;
    GLuint slot;
    std::string location;
    int width, height;

public:
    Texture(TextureInfo info, GLuint slot);

    void bind();

    ~Texture();

    static void unbind();

    GLuint getId();

    GLuint getSlot();

    std::string getLocation();

    int getHeight();

    int getWidth();
};

#endif