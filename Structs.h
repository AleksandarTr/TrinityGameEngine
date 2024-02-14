#ifndef TRINITYENGINE_STRUCTS_H
#define TRINITYENGINE_STRUCTS_H

#include <glm/glm.hpp>
#include <string>

struct Vertex {
    glm::vec3 position = glm::vec3(0);
    glm::vec3 color = glm::vec3(0);
    glm::vec3 texPosition = glm::vec3(0);
    glm::vec3 lightNormal = glm::vec3(0);
    glm::vec3 tangent = glm::vec3(1);
    glm::vec3 bitangent = glm::vec3(1);
};

enum struct LightingType {PointLight, DirectionalLight, SpotLight};

struct TextureInfo {
    std::string location = "";
    std::string format = "image/jpeg";
    int magFilter = GL_NEAREST;
    int minFilter = GL_NEAREST;
    int wrapS = GL_REPEAT;
    int wrapT = GL_REPEAT;
};

#endif
