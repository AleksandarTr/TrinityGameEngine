#include <iostream>
#include "View.h"
#include "Shader.h"
#include <algorithm>

View* View::activeView = nullptr;

View::View(unsigned int width, unsigned int height, float fov, float nearPlane, float farPlane)
: width(width), height(height), fov(fov), nearPlane(nearPlane), farPlane(farPlane), orthographic(false) {}

View::View(unsigned int width, unsigned int height, float left, float right, float bottom, float top, float nearPlane, float farPlane)
        : width(width), height(height), nearPlane(nearPlane), farPlane(farPlane), left(left), right(right), bottom(bottom), top(top), orthographic(true) {}

void View::updateMatrix() {
    glm::mat4 view = glm::lookAt(getPosition(), getPosition() + orientation, up);
    glm::mat4 proj;
    if(orthographic)
        proj = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    else
        proj = glm::perspective(glm::radians(fov), 1.0f * width / height, nearPlane, farPlane);

    cameraMat = proj * view;
    calculateCameraFrustum();
}

void View::setFov(float fov) {
    fov = fov;
    orthographic = false;
    movedFlag = true;
}

void View::setNearPlane(float nearPlane) {
    this->nearPlane = nearPlane;
    movedFlag = true;
}

void View::setFarPlane(float farPlane) {
    this->farPlane = farPlane;
    movedFlag = true;
}

glm::mat4 View::getCameraMatrix() const {
    return cameraMat;
}

View &View::operator=(const View &camera) {
    if(this == &camera) return *this;

    fov = camera.fov;
    nearPlane = camera.nearPlane;
    farPlane = camera.farPlane;

    orientation = camera.orientation;
    up = camera.up;
    cameraMat = camera.cameraMat;

    movedFlag = true;
    return *this;
}

void View::activate() {
    activeView = this;
    if(movedFlag) {
        updateMatrix();
        movedFlag = false;
    }
}

View *View::getActiveView() {
    return activeView;
}

void View::calculateCameraFrustum() {
    glm::mat4 &m = cameraMat;
    frustum[FrustumSide::FrustumFront] = {.normal = glm::normalize(orientation)};
    frustum[FrustumSide::FrustumFront].d = -glm::dot(frustum[FrustumSide::FrustumFront].normal, getPosition() + nearPlane * orientation);

    frustum[FrustumSide::FrustumBack] = {.normal = -frustum[FrustumSide::FrustumFront].normal};
    frustum[FrustumSide::FrustumBack].d = -glm::dot(frustum[FrustumSide::FrustumBack].normal, getPosition() + farPlane * orientation);

    frustum[FrustumSide::FrustumLeft] = {.normal = glm::normalize(glm::vec3(m[0][3] + m[0][0], m[1][3] + m[1][0], m[2][3] + m[2][0]))};
    frustum[FrustumSide::FrustumLeft].d = -glm::dot(frustum[FrustumSide::FrustumLeft].normal, getPosition());

    frustum[FrustumSide::FrustumRight] = {.normal = glm::normalize(glm::vec3(m[0][3] - m[0][0], m[1][3] - m[1][0], m[2][3] - m[2][0]))};
    frustum[FrustumSide::FrustumRight].d = -glm::dot(frustum[FrustumSide::FrustumRight].normal, getPosition());

    frustum[FrustumSide::FrustumBottom] = {.normal = glm::normalize(glm::vec3(m[0][3] + m[0][1], m[1][3] + m[1][1], m[2][3] + m[2][1]))};
    frustum[FrustumSide::FrustumBottom].d = -glm::dot(frustum[FrustumSide::FrustumBottom].normal, getPosition());

    frustum[FrustumSide::FrustumTop] = {.normal = glm::normalize(glm::vec3(m[0][3] - m[0][1], m[1][3] - m[1][1], m[2][3] - m[2][1]))};
    frustum[FrustumSide::FrustumTop].d = -glm::dot(frustum[FrustumSide::FrustumTop].normal, getPosition());
}

const View::Plane *View::getViewFrustum() const {
    return frustum;
}

glm::vec3 View::getOrientation() const {
    return orientation;
}

void View::setOrientation(glm::vec3 orientation) {
    this->orientation = orientation;
    movedFlag = true;
}

void View::setLeftBorder(float value) {
    left = value;
    orthographic = true;
    movedFlag = true;
}

void View::setRightBorder(float value) {
    right = value;
    orthographic = true;
    movedFlag = true;
}

void View::setBottomBorder(float value) {
    bottom = value;
    orthographic = true;
    movedFlag = true;
}

void View::setTopBorder(float value) {
    top = value;
    orthographic = true;
    movedFlag = true;
}

void View::setOrthographicBorder(float left, float right, float bottom, float top) {
    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
    orthographic = true;
    movedFlag = true;
}

float View::getFov() const {
    return fov;
}

float View::getNearPlane() const {
    return nearPlane;
}

float View::getFarPlane() const {
    return farPlane;
}

void View::setUp(glm::vec3 value) {
    up = value;
    movedFlag = true;
}

glm::vec3 View::getUp() const {
    return up;
}

float View::getWidth() const {
    return width;
}

float View::getHeight() const {
    return height;
}
