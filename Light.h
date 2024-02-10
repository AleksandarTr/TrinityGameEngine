#ifndef TRINITYENGINE_LIGHT_H
#define TRINITYENGINE_LIGHT_H

#include "MultiTextureMesh.h"

class Light : public MultiTextureMesh {
private:
    glm::vec3 color;
    glm::vec3 direction;
    LightingType type;
    bool useLightColor = true;
public:
    Light(std::vector<Vertex> &vertices, std::vector<Index> &indices, Shader &shader, glm::vec3 color, glm::vec3 direction, LightingType type,
          std::vector<std::string> diffuseTextures = std::vector<std::string>(), std::vector<std::string> specularTextures = std::vector<std::string>());

    const glm::vec3 &getColor() const;

    void setColor(const glm::vec3 color);

    const glm::vec3 &getDirection() const;

    void setDirection(const glm::vec3 direction);

    LightingType getType() const;

    void setType(LightingType type);

    void initializeOtherFields() override;

    bool doesUseLightColor() const;

    void setUseLightColor(bool useLightColor);
};

#endif
