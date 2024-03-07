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
    glm::vec3 getPosition(); //Get the global position of an object

    glm::quat getRotation(); //Get the global rotation of an object

    virtual void rotate(glm::vec3 direction); //Change the global rotation of an object by given amounts

    virtual void rotate(glm::quat direction); //Rotate the present rotation by given amount

    virtual void move(glm::vec3 direction); //Translate the global position of an object

    void update(double timeDelta); //Update all the parameters in the object based on time passed

    void setVelocity(glm::vec3 velocity); //Set the velocity of the object, which is applied with void update(double timeDelta)

    void setAcceleration(glm::vec3 acceleration); //Set the acceleration of the object, which is applied with void update(double timeDelta)

    void increaseVelocity(glm::vec3 increment); //Increase the velocity of the object, which is applied with void update(double timeDelta)

    void increaseAcceleration(glm::vec3 increment); //Increase the acceleration of the object, which is applied with void update(double timeDelta)

    void setAngularVelocity(glm::vec3 velocity); //Set the angular velocity of the object, which is applied with void update(double timeDelta)

    void setAngularAcceleration(glm::vec3 acceleration); //Set the angular acceleration of the object, which is applied with void update(double timeDelta)

    void increaseAngularVelocity(glm::vec3 increment); //Increase the angular velocity of the object, which is applied with void update(double timeDelta)

    void increaseAngularAcceleration(glm::vec3 increment); //Increase the angular acceleration of the object, which is applied with void update(double timeDelta)

    void setPosition(glm::vec3 position); //Set the global position of an object

    void setRotation(glm::quat rotation); //Set the global rotation of an object

    virtual void setScale(glm::vec3 scale); //Set the absolute global scale of an object

    virtual void scale(glm::vec3 scaling); //Apply scaling to an object

    glm::vec3 getScale(); //Get the global scale of an object
};

#endif
