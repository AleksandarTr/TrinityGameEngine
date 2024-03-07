#ifndef TRINITYENGINE_CAMERA_H
#define TRINITYENGINE_CAMERA_H

#include "View.h"

class Camera : public View {
private:
    float sensitivity = 500.0f; // Mouse sensitivity
    float speed = 10.0f; // Movement speed

public:
    Camera(int width, int height, float fov = 45, float nearPlane = 0.1f, float farPlane = 100);
    Camera(int width, int height, float left, float right, float bottom, float top, float nearPlane = 0.1f, float farPlane = 100);

    virtual void controls(GLFWwindow* window); //Function for receiving keyboard and mouse inputs and moving the camera accordingly

    void setSensitivity(float value); //Set sensitivity without any checks
    float getSensitivity() const;

    void setSpeed(float value); //Set camera movement speed without checks
    float getSpeed() const;
};

#endif
