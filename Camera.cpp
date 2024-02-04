#include <iostream>
#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position) : width(width), height(height) {
    move(position);
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane) {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);

    view = glm::lookAt(getPosition(), getPosition() + orientation, up);
    proj = glm::perspective(glm::radians(FOVdeg), 1.0f * width / height, nearPlane, farPlane);

    cameraMat = proj * view;
}

void Camera::setMatrix(Shader &shader, const char *uniform) {
    shader.activate();
    glUniformMatrix4fv(glGetUniformLocation(shader.getProgramID(), uniform), 1, GL_FALSE, glm::value_ptr(cameraMat));
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

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float rotY = sensitivity * (float) (mouseY - (height / 2)) / height;
    float rotX = sensitivity * (float) (mouseX - (width / 2)) / width;

    orientation = glm::rotate(orientation, glm::radians(-rotY), glm::normalize(glm::cross(orientation, up)));
    orientation = glm::rotate(orientation, glm::radians(-rotX), up);

    glfwSetCursorPos(window, width/2, height/2);
}
