#include "Movable.h"

void Movable::rotate(glm::vec3 angles) {
    rotation += angles;
    movedFlag = true;
}

void Movable::move(glm::vec3 direction) {
    position += direction;
    movedFlag = true;
}

glm::vec3 Movable::getPosition() {
    return position;
}

glm::vec3 Movable::getRotation() {
    return rotation;
}

void Movable::update(double timeDelta) {
    if(angularVelocity != glm::vec3(0) || position != glm::vec3(0)) {
        rotation.x += angularVelocity.x * timeDelta + angularAcceleration.x / 2 * timeDelta * timeDelta;
        rotation.y += angularVelocity.y * timeDelta + angularAcceleration.y / 2 * timeDelta * timeDelta;
        rotation.z += angularVelocity.z * timeDelta + angularAcceleration.z / 2 * timeDelta * timeDelta;

        angularVelocity.x += angularAcceleration.x * timeDelta;
        angularVelocity.y += angularAcceleration.y * timeDelta;
        angularVelocity.z += angularAcceleration.z * timeDelta;

        movedFlag = true;
    }

    if(velocity != glm::vec3(0) || acceleration != glm::vec3(0)) {
        position.x += velocity.x * timeDelta + acceleration.x / 2 * timeDelta * timeDelta;
        position.y += velocity.y * timeDelta + acceleration.y / 2 * timeDelta * timeDelta;
        position.z += velocity.z * timeDelta + acceleration.z / 2 * timeDelta * timeDelta;

        velocity.x += acceleration.x * timeDelta;
        velocity.y += acceleration.y * timeDelta;
        velocity.z += acceleration.z * timeDelta;

        movedFlag = true;
    }
}

void Movable::setVelocity(glm::vec3 velocity) {
    this->velocity = velocity;
}

void Movable::setAcceleration(glm::vec3 acceleration) {
    this->acceleration = acceleration;
}

void Movable::incVelocity(glm::vec3 increment) {
    velocity += increment;
}

void Movable::incAcceleration(glm::vec3 increment) {
    acceleration += increment;
}

void Movable::setAngularVelocity(glm::vec3 velocity) {
    angularVelocity = velocity;
}

void Movable::setAngularAcceleration(glm::vec3 acceleration) {
    angularAcceleration = acceleration;
}

void Movable::incAngularVelocity(glm::vec3 increment) {
    angularVelocity += increment;
}

void Movable::incAngularAcceleration(glm::vec3 increment) {
    angularAcceleration += increment;
}