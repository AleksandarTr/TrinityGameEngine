#include "MoveableObject.h"

void MoveableObject::rotate(glm::vec3 angles) {
    rotation += angles;
}

void MoveableObject::move(glm::vec3 direction) {
    position += direction;
}

glm::vec3 MoveableObject::getPosition() {
    return position;
}

glm::vec3 MoveableObject::getRotation() {
    return rotation;
}

void MoveableObject::update(double timeDelta) {
    rotation.x += angularVelocity.x * timeDelta + angularAcceleration.x / 2 * timeDelta * timeDelta;
    rotation.y += angularVelocity.y * timeDelta + angularAcceleration.y / 2 * timeDelta * timeDelta;
    rotation.z += angularVelocity.z * timeDelta + angularAcceleration.z / 2 * timeDelta * timeDelta;

    angularVelocity.x += angularAcceleration.x * timeDelta;
    angularVelocity.y += angularAcceleration.y * timeDelta;
    angularVelocity.z += angularAcceleration.z * timeDelta;

    position.x += velocity.x * timeDelta + acceleration.x / 2 * timeDelta * timeDelta;
    position.y += velocity.y * timeDelta + acceleration.y / 2 * timeDelta * timeDelta;
    position.z += velocity.z * timeDelta + acceleration.z / 2 * timeDelta * timeDelta;

    velocity.x += acceleration.x * timeDelta;
    velocity.y += acceleration.y * timeDelta;
    velocity.z += acceleration.z * timeDelta;
}

void MoveableObject::setVelocity(glm::vec3 velocity) {
    this->velocity = velocity;
}

void MoveableObject::setAcceleration(glm::vec3 acceleration) {
    this->acceleration = acceleration;
}

void MoveableObject::incVelocity(glm::vec3 increment) {
    velocity += increment;
}

void MoveableObject::incAcceleration(glm::vec3 increment) {
    acceleration += increment;
}

void MoveableObject::setAngularVelocity(glm::vec3 velocity) {
    angularVelocity = velocity;
}

void MoveableObject::setAngularAcceleration(glm::vec3 acceleration) {
    angularAcceleration = acceleration;
}

void MoveableObject::incAngularVelocity(glm::vec3 increment) {
    angularVelocity += increment;
}

void MoveableObject::incAngularAcceleration(glm::vec3 increment) {
    angularAcceleration += increment;
}