#ifndef TRINITYENGINE_TEXTURE_H
#define TRINITYENGINE_TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include <glad/glad.h>

class Texture {
private:
    GLuint textureId = 0;
    GLuint slot;

public:
    Texture(std::string location, GLuint slot, GLenum format);

    void bind();

    ~Texture();

    static void unbind();

    GLuint getId();

    GLuint getSlot();
};

#endif