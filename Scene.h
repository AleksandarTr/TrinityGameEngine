#ifndef TRINITYENGINE_SCENE_H
#define TRINITYENGINE_SCENE_H

#include <vector>
#include "Movable.h"
class Model;

class Scene : public Movable {
public:
    std::vector<Model*> models = std::vector<Model*>();

public:
    void addModel(Model& model);

    void removeModel(Model& model);

    void removeModel(int index = -1);

    void operator+=(Model& model);

    void operator-=(Model& model);

    Model& operator[] (int index);

    Model& at(int index);

    void draw();
};

#endif
