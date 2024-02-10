#ifndef TRINITYENGINE_STRUCTS_H
#define TRINITYENGINE_STRUCTS_H

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 color = glm::vec3(0);
    glm::vec3 texPosition = glm::vec3(0);
    glm::vec3 lightNormal = glm::vec3(0);
};

enum struct LightingType {PointLight, DirectionalLight, SpotLight};

#endif
