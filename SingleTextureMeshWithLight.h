#ifndef TRINITYENGINE_SINGLETEXTUREMESHWITHLIGHT_H
#define TRINITYENGINE_SINGLETEXTUREMESHWITHLIGHT_H

#include "Light.h"
#include "SingleTextureMesh.h"

class SingleTextureMeshWithLight : public SingleTextureMesh {
private:
    std::vector<Light*> lights = std::vector<Light*>();

public:
    SingleTextureMeshWithLight(std::vector<Vertex> vertices, std::vector<Index> indices, Shader &shader,
           std::string diffuseTexture = "", std::string specularTexture = "");

    void addLight(Light& light);

    void removeLight(int index = -1);

    void initializeOtherFields() override;
};


#endif