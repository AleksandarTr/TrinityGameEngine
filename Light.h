#ifndef TRINITYENGINE_LIGHT_H
#define TRINITYENGINE_LIGHT_H

#include "SingleTextureMesh.h"

class Light : public Movable {
private:
    glm::vec3 color;
    glm::vec3 direction;
    LightingType type;

    GLuint shadowBuffer;
    Texture *shadowMap;
    glm::mat4 lightMatrix;

public:
    const static unsigned int shadowWidth = 1024, shadowHeight = 1024;

    Light(glm::vec3 color, glm::vec3 direction, LightingType type);

    const glm::vec3 &getColor() const;

    void setColor(const glm::vec3 color);

    const glm::vec3 &getDirection() const;

    void setDirection(const glm::vec3 direction);

    LightingType getType() const;

    void setType(LightingType type);

    void drawShadowMap();

    Texture& getShadowMap();

    void loadLight(int index);

    const glm::mat4 &getLightMatrix();
};

#endif
