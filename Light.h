#ifndef TRINITYENGINE_LIGHT_H
#define TRINITYENGINE_LIGHT_H

#include "SingleTextureMesh.h"

class Light : public View {
private:
    glm::vec3 color;
    LightingType type;

    GLuint shadowBuffer;
    Texture *shadowMap;
    const unsigned int shadowWidth, shadowHeight;

public:
    Light(glm::vec3 color, glm::vec3 direction, LightingType type, unsigned int shadowWidth = 1024, unsigned int shadowHeight = 1024);

    glm::vec3 getColor() const;
    void setColor(glm::vec3 color);

    LightingType getType() const;
    void setType(LightingType type);
    void setType(LightingType type, float nearPlane, float farPlane, float left_fov, float right = 0, float bottom = 0, float top = 0);

    void drawShadowMap();

    Texture& getShadowMap() const;

    void loadLight(int index);

    unsigned int getShadowWidth() const;
    unsigned int getShadowHeight() const;
};

#endif
