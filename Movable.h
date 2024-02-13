#ifndef TRINITYENGINE_MOVEABLE_H
#define TRINITYENGINE_MOVEABLE_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class Movable {
private:
    glm::vec3 position = glm::vec3(0);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scaling = glm::vec3(1);

    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 angularVelocity = glm::vec3(0);

    glm::vec3 acceleration = glm::vec3(0);
    glm::vec3 angularAcceleration = glm::vec3(0);

protected:
    bool movedFlag = false;

public:
    glm::vec3 getPosition();

    glm::quat getRotation();

    virtual void rotate(glm::vec3 direction);

    virtual void rotate(glm::quat direction);

    virtual void move(glm::vec3 direction);

    void update(double timeDelta);

    void setVelocity(glm::vec3 velocity);

    void setAcceleration(glm::vec3 acceleration);

    void incVelocity(glm::vec3 increment);

    void incAcceleration(glm::vec3 increment);

    void setAngularVelocity(glm::vec3 velocity);

    void setAngularAcceleration(glm::vec3 acceleration);

    void incAngularVelocity(glm::vec3 increment);

    void incAngularAcceleration(glm::vec3 increment);

    void setPosition(glm::vec3 position);

    void setRotation(glm::quat rotation);

    void setScale(glm::vec3 scale);

    virtual void scale(glm::vec3 scaling);

    glm::vec3 getScale();
};

#endif
