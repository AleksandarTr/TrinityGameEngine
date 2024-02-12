#include "Movable.h"

void Movable::rotate(glm::vec3 angles) {
    rotation = glm::quat(angles) * rotation;
    if(angles != glm::vec3(0)) movedFlag = true;
}

void Movable::move(glm::vec3 direction) {
    position += direction;
    if(direction != glm::vec3(0)) movedFlag = true;
}

glm::vec3 Movable::getPosition() {
    return position;
}

glm::quat Movable::getRotation() {
    return rotation;
}

void Movable::update(double timeDelta) {
    if(angularVelocity != glm::vec3(0) || position != glm::vec3(0)) {
        glm::vec3 angles = float(timeDelta) * angularVelocity + float(timeDelta * timeDelta / 2) * angularAcceleration;
        rotation = glm::quat(angles) * rotation;
        angularVelocity += float(timeDelta) * angularAcceleration;

        movedFlag = true;
    }

    if(velocity != glm::vec3(0) || acceleration != glm::vec3(0)) {
        position += float(timeDelta) * velocity + float(timeDelta * timeDelta / 2) * acceleration;
        velocity += float(timeDelta) * acceleration;

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

void Movable::rotate(glm::quat direction) {
    rotation = direction * rotation;
    if(direction != glm::quat(1, 0, 0, 0)) movedFlag = true;
}

void Movable::setPosition(glm::vec3 position) {
    if(this->position != position) movedFlag = true;
    this->position = position;
}

void Movable::setRotation(glm::quat rotation) {
    if(this->rotation != rotation) movedFlag = true;
    this->rotation = rotation;
}

void Movable::setScale(glm::vec3 scale) {
    if(this->scaling != scale) movedFlag = true;
    this->scaling = scale;
}

void Movable::scale(glm::vec3 scaling) {
    if(scaling != glm::vec3(1)) movedFlag = true;
    this->scaling *= scaling;
}

glm::vec3 Movable::getScale() {
    return scaling;
}
