#ifndef TRINITYENGINE_LIGHT_H
#define TRINITYENGINE_LIGHT_H

#include "SingleTextureMesh.h"

class Light : public Movable {
private:
    glm::vec3 color;
    glm::vec3 direction;
    LightingType type;

public:
    Light(glm::vec3 color, glm::vec3 direction, LightingType type);

    const glm::vec3 &getColor() const;

    void setColor(const glm::vec3 color);

    const glm::vec3 &getDirection() const;

    void setDirection(const glm::vec3 direction);

    LightingType getType() const;

    void setType(LightingType type);
};

#endif
