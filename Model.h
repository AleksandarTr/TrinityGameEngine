#ifndef TRINITYENGINE_MODEL_H
#define TRINITYENGINE_MODEL_H

#include "Mesh.h"

class Model {
private:
    std::vector<Mesh*> meshes;

public:
    Model();
};

#endif
