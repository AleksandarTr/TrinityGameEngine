#ifndef TRINITYENGINE_MOVEABLE_H
#define TRINITYENGINE_MOVEABLE_H

#include <glm/glm.hpp>

class Movable {
private:
    glm::vec3 position = glm::vec3(0);
    glm::vec3 rotation = glm::vec3(0);

    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 angularVelocity = glm::vec3(0);

    glm::vec3 acceleration = glm::vec3(0);
    glm::vec3 angularAcceleration = glm::vec3(0);

protected:
    bool movedFlag = false;

public:
    glm::vec3 getPosition();

    glm::vec3 getRotation();

    void rotate(glm::vec3 direction);

    void move(glm::vec3 direction);

    void update(double timeDelta);

    void setVelocity(glm::vec3 velocity);

    void setAcceleration(glm::vec3 acceleration);

    void incVelocity(glm::vec3 increment);

    void incAcceleration(glm::vec3 increment);

    void setAngularVelocity(glm::vec3 velocity);

    void setAngularAcceleration(glm::vec3 acceleration);

    void incAngularVelocity(glm::vec3 increment);

    void incAngularAcceleration(glm::vec3 increment);
};

#endif
