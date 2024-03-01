#ifndef TRINITYENGINE_CAMERA_H
#define TRINITYENGINE_CAMERA_H

#include "View.h"

class Camera : public View {
private:
    float sensitivity = 500.0f;
    float speed = 10.0f;

public:
    Camera(int width, int height, float fov = 45, float nearPlane = 0.1f, float farPlane = 100);
    Camera(int width, int height, float left, float right, float bottom, float top, float nearPlane = 0.1f, float farPlane = 100);

    void controls(GLFWwindow* window);

    void setSensitivity(float value);
    float getSensitivity() const;

    void setSpeed(float value);
    float getSpeed() const;
};

#endif
