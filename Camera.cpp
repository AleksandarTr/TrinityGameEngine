#include <iostream>
#include "Camera.h"
#include "Shader.h"
#include <algorithm>

Camera::Camera(int width, int height, glm::vec3 position) : width(width), height(height) {
    move(position);
}

void Camera::updateMatrix() {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::lookAt(getPosition(), getPosition() + orientation, up);
    proj = glm::perspective(glm::radians(FOV), 1.0f * width / height, nearPlane, farPlane);

    cameraMat = proj * view;
    for(auto shader : shaders) shader->cameraMovedFlag = true;
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
}

void Camera::setNearPlane(float nearPlane) {
    this->nearPlane = nearPlane;
}

void Camera::setFarPlane(float farPlane) {
    this->farPlane = farPlane;
}

void Camera::addShader(Shader &shader) {
    if(std::find(shaders.begin(), shaders.end(), &shader) != shaders.end()) return;
    shaders.push_back(&shader);
}

void Camera::removeShader(int index) {
    if(index == -1) {
        shaders.pop_back();
        return;
    }

    shaders.erase(shaders.begin() + index);
}

void Camera::removeShader(Shader &shader) {
    auto position = std::find(shaders.begin(), shaders.end(), &shader);
    if(position == shaders.end()) return;

    shaders.erase(position);
}

glm::mat4 Camera::getCameraMatrix() {
    return cameraMat;
}


