#ifndef TRINITYENGINE_OBJECT_H
#define TRINITYENGINE_OBJECT_H

#include "Light.h"

class Object : public Mesh {
private:
    std::vector<Light*> lights;

public:
    Object(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader,
           std::vector<std::string> diffuseTextures = std::vector<std::string>(), std::vector<std::string> specularTextures = std::vector<std::string>());

    void addLight(Light& light);

    void removeLight(int index = -1);

    void initializeOtherFields() override;
};


#endif