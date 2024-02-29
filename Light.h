#ifndef TRINITYENGINE_LIGHT_H
#define TRINITYENGINE_LIGHT_H

#include "SingleTextureMesh.h"

class Light : public Camera {
private:
    glm::vec3 color;
    LightingType type;

    GLuint shadowBuffer;
    Texture *shadowMap;

public:
    const static unsigned int shadowWidth = 1024, shadowHeight = 1024;

    Light(glm::vec3 color, glm::vec3 direction, LightingType type);

    const glm::vec3 &getColor() const;

    void setColor(const glm::vec3 color);

    LightingType getType() const;

    void setType(LightingType type);

    void setType(LightingType type, float nearPlane, float farPlane, float left_fov, float right = 0, float bottom = 0, float top = 0);

    void drawShadowMap();

    Texture& getShadowMap();

    void loadLight(int index);

    void setPerspective(float fov, float nearPlane, float farPlane);

    void setOrthographic(float left, float right, float top, float bottom, float nearPlane, float farPlane);
};

#endif
