#ifndef TRINITYENGINE_TEXTURE_H
#define TRINITYENGINE_TEXTURE_H

#include <glad/glad.h>
#include <stb/stb_image.h>
#include <string>
#include <gl/gl.h>

class Texture {
private:
    GLuint textureId;
    GLuint slot;

public:
    Texture(std::string location, GLuint slot, GLenum format);

    void bind();

    ~Texture();
};

#endif