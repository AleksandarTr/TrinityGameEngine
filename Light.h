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
    void setColor(glm::vec3 color); //Set the color of light without any checks

    LightingType getType() const;
    void setType(LightingType type); //Set the type of the light and load default projection values for the view parent class
    void setType(LightingType type, float nearPlane, float farPlane, float left_fov, float right = 0, float bottom = 0, float top = 0); // Set type of light and manually load projection values for the view parent class

    void drawShadowMap(); //Render the shadow map in the form of a depth frame buffer

    Texture& getShadowMap() const; //Get the shadow map texture member

    void loadLight(int index); //Initialize all light parameters for light for rendering

    unsigned int getShadowWidth() const; //Get the width of the shadow map texture
    unsigned int getShadowHeight() const; //Get the height of the shadow map texture

    void setAttenuationParams(float a, float b); // Set the parameters for calculating attenuation calculated as lightLevel / (a * distance ^ 2 + b * distance + 1)
    void setSpotlightParams(float innerCone, float outerCone); // Set parameters for inner and outer circle which the spotlight illuminates

    void setFov(float value); //Set view fov and adjust spotlight parameters
};

#endif
