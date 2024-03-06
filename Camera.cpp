#include "Camera.h"

void Camera::setSensitivity(float value) {
    sensitivity = value;
}

float Camera::getSensitivity() const {
    return sensitivity;
}

void Camera::setSpeed(float value) {
    speed = value;
}

float Camera::getSpeed() const {
    return speed;
}

void Camera::controls(GLFWwindow *window) {
    glm::vec3 direction(0);
    glm::vec3 orientation = getOrientation();
    glm::vec3 up = getUp();
    float width = getWidth();
    float height = getHeight();

    //Forward and backward movement
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        direction += -glm::normalize(glm::cross(orientation, up));
    else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction += glm::normalize(glm::cross(orientation, up));

    //Left and right movement
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        direction += orientation;
    else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        direction += -orientation;

    //Up and down movement
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        direction += up;
    else if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        direction += -up;

    //Setting the velocity according to the direction and speed, where the direction is normalized to avoid faster diagonal movement
    setVelocity(direction == glm::vec3(0) ? glm::vec3(0) : speed * glm::normalize(direction));

    //Hiding teh cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    //Turning off the window on right click (For development purposes only)
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    //Calculating x and y rotation based on how far the mouse has moved from the center of the screen
    float rotY = sensitivity * (float) (mouseY - (height / 2)) / height;
    float rotX = sensitivity * (float) (mouseX - (width / 2)) / width;

    //Eliminate wonky movement when looking straight up and straight down
    if((orientation.y < 0.999 || rotY > 0) && (orientation.y > -0.999 || rotY < 0))
        orientation = glm::rotate(orientation, glm::radians(-rotY), glm::normalize(glm::cross(orientation, up)));

    //Signal to update the view matrix when the mouse has moved
    if(rotX != 0 || rotY != 0) movedFlag = true;

    //Updating orientation and returning the mouse to the center of the screen
    setOrientation(glm::rotate(orientation, glm::radians(-rotX), up));
    glfwSetCursorPos(window, width/2, height/2);
}

Camera::Camera(int width, int height, float fov, float nearPlane, float farPlane)
: View(width, height, fov, nearPlane, farPlane) {}

Camera::Camera(int width, int height, float left, float right, float bottom, float top, float nearPlane, float farPlane)
        : View(width, height, left, right, bottom, top, nearPlane, farPlane) {}
