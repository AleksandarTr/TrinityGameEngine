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
    TextureInfo info;

public:
    Texture(TextureInfo info);

    void bind(GLenum target = GL_TEXTURE_2D) const;

    ~Texture();

    static void unbind();

    TextureInfo& getInfo();

    GLuint getId() const;
    void setId(GLuint id);
};

#endif