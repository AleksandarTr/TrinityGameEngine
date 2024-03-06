#ifndef TRINITYENGINE_LIGHT_H
#define TRINITYENGINE_LIGHT_H

#include "SingleTextureMesh.h"

class Light : public View {
private:
    glm::vec3 color;
    LightingType type;
    float a = 1, b = 0; // Parameters used in the formula lightLevel / (a * distance^2 + b * distance + 1) used for calculating attenuation
    float innerCone = .999, outerCone = .99; // Parameters for spotlights to determine the circle they will light up

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

    void loadLight(int index); //Initialize all light parameters for light for rendering

    unsigned int getShadowWidth() const;
    unsigned int getShadowHeight() const;

    void setAttenuationParams(float a, float b); // Attenuation is calculated as lightLevel / (a * distance ^ 2 + b * distance + 1)
    void setSpotlightParams(float innerCone, float outerCone);

    void setFov(float value);
};

#endif
