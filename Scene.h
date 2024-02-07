#ifndef TRINITYENGINE_SCENE_H
#define TRINITYENGINE_SCENE_H

#include <vector>
#include "Movable.h"

class Model;

class Scene : public Movable {
private:
    std::vector<Model*> models;

public:
    void addModel(Model& model);

    void removeModel(Model& model);

    void removeModel(int index = -1);

    void operator+=(Model& model);

    void operator-=(Model& model);

    Model& operator[] (int index);

    Model& at(int index);
};

#endif
