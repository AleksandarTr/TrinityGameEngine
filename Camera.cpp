#include <iostream>
#include "Camera.h"
#include "Shader.h"
#include <algorithm>

Camera* Camera::activeCamera = nullptr;

Camera::Camera(int width, int height, glm::vec3 position) : width(width), height(height) {
    Movable::move(position);
    calculateCameraFrustum();
}

void Camera::updateMatrix() {
    glm::mat4 view;
    glm::mat4 proj;

    view = glm::lookAt(getPosition(), getPosition() + orientation, up);
    if(orthographic)
        proj = glm::ortho(projectionLeft, projectionRight, projectionBottom, projectionTop, nearPlane, farPlane);
    else
        proj = glm::perspective(glm::radians(FOV), 1.0f * width / height, nearPlane, farPlane);

    cameraMat = proj * view;
    calculateCameraFrustum();
}

void Camera::inputs(GLFWwindow *window) {
    glm::vec3 direction(0);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction += -glm::normalize(glm::cross(orientation, up));
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction += glm::normalize(glm::cross(orientation, up));

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction += orientation;
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction += -orientation;

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        direction += up;
    else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        direction += -up;

    setVelocity(direction == glm::vec3(0) ? glm::vec3(0) : speed * glm::normalize(direction));

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float rotY = sensitivity * (float) (mouseY - (height / 2)) / height;
    float rotX = sensitivity * (float) (mouseX - (width / 2)) / width;

    if((orientation.y < 0.9999 || rotY > 0) && (orientation.y > -0.9999 || rotY < 0))
        orientation = glm::rotate(orientation, glm::radians(-rotY), glm::normalize(glm::cross(orientation, up)));

    if(rotX != 0 || rotY != 0) movedFlag = true;

    orientation = glm::rotate(orientation, glm::radians(-rotX), up);

    glfwSetCursorPos(window, width/2, height/2);
    if(movedFlag) {
        movedFlag = false;
        updateMatrix();
    }
}

void Camera::setFov(float fov) {
    FOV = fov;
    movedFlag = true;
}

void Camera::setNearPlane(float nearPlane) {
    this->nearPlane = nearPlane;
    movedFlag = true;
}

void Camera::setFarPlane(float farPlane) {
    this->farPlane = farPlane;
    movedFlag = true;
}

glm::mat4 Camera::getCameraMatrix() {
    return cameraMat;
}

Camera &Camera::operator=(const Camera &camera) {
    if(this == &camera) return *this;

    FOV = camera.FOV;
    nearPlane = camera.nearPlane;
    farPlane = camera.farPlane;

    sensitivity = camera.sensitivity;
    speed = camera.speed;

    orientation = camera.orientation;
    up = camera.up;
    cameraMat = camera.cameraMat;

    movedFlag = true;
    return *this;
}

void Camera::activateCamera() {
    activeCamera = this;
    if(movedFlag) {
        updateMatrix();
        movedFlag = false;
    }
}

Camera *Camera::getActiveCamera() {
    return activeCamera;
}

void Camera::calculateCameraFrustum() {
    glm::mat4 &m = cameraMat;
    frustum[FrustumSide::front] = {.normal = glm::normalize(orientation)};
    frustum[FrustumSide::front].d = -glm::dot(frustum[FrustumSide::front].normal, getPosition() + nearPlane * orientation);
    frustum[FrustumSide::back] = {.normal = -frustum[FrustumSide::front].normal};
    frustum[FrustumSide::back].d = -glm::dot(frustum[FrustumSide::back].normal, getPosition() + farPlane * orientation);
    frustum[FrustumSide::left] = {.normal = glm::normalize(glm::vec3(m[0][3]+m[0][0], m[1][3]+m[1][0], m[2][3]+m[2][0])), .d = m[3][3]+m[3][0]};
    frustum[FrustumSide::right] = {.normal = glm::normalize(glm::vec3(m[0][3]-m[0][0], m[1][3]-m[1][0], m[2][3]-m[2][0])), .d = m[3][3]-m[3][0]};
    frustum[FrustumSide::bottom] = {.normal = glm::normalize(glm::vec3(m[0][3]+m[0][1], m[1][3]+m[1][1], m[2][3]+m[2][1])), .d = m[3][3]+m[3][1]};
    frustum[FrustumSide::top] = {.normal = glm::normalize(glm::vec3(m[0][3]-m[0][1], m[1][3]-m[1][1], m[2][3]-m[2][1])), .d = m[3][3]-m[3][1]};
}

Camera::Plane *Camera::getViewFrustum() {
    return frustum;
}

glm::vec3 Camera::getOrientation() const {
    return orientation;
}

void Camera::setOrientation(glm::vec3 orientation) {
    this->orientation = orientation;
    movedFlag = true;
}

void Camera::setMode(bool isOrthographic) {
    orthographic = isOrthographic;
    movedFlag = true;
}

void Camera::setLeftBorder(float value) {
    projectionLeft = value;
    movedFlag = true;
}

void Camera::setRightBorder(float value) {
    projectionRight = value;
    movedFlag = true;
}

void Camera::setBottomBorder(float value) {
    projectionBottom = value;
    movedFlag = true;
}

void Camera::setTopBorder(float value) {
    projectionTop = value;
    movedFlag = true;
}