#ifndef TRINITYENGINE_CAMERA_H
#define TRINITYENGINE_CAMERA_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "Movable.h"
#include <vector>

class Shader;

class Camera : public Movable {
private:
    glm::vec3 orientation = glm::vec3(0,0,-1.0f);
    glm::vec3 up = glm::vec3(0, 1.0f, 0);
    glm::mat4 cameraMat = glm::mat4(1);

    int width;
    int height;

    float sensitivity = 500.0f;
    float speed = 10.0f;

    float FOV = 45;
    float nearPlane = 0.1f;
    float farPlane = FLT_MAX;

public:
    Camera(int width, int height, glm::vec3 position);

    Camera& operator=(const Camera& camera);

    void updateMatrix();

    void inputs(GLFWwindow* window);

    void setFov(float fov);

    void setNearPlane(float nearPlane);

    void setFarPlane(float farPlane);

    glm::mat4 getCameraMatrix();
};

#endif
