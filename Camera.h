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
public:
    struct Plane {
        glm::vec3 point;
        glm::vec3 normal;
    };

private:
    glm::vec3 orientation = glm::vec3(0,0,-1.0f);
    glm::vec3 up = glm::vec3(0, 1.0f, 0);
    glm::mat4 cameraMat = glm::mat4(1);
    bool orthographic = false;

    int width;
    int height;

    float sensitivity = 500.0f;
    float speed = 10.0f;

    union {
        float FOV = 45;
        float projectionLeft;
    };
    float projectionRight = 0;
    float projectionBottom = -10;
    float projectionTop = 10;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;


    Plane frustum[6];
    void calculateCameraFrustum();

    static Camera *activeCamera;

public:
    enum FrustumSide {
        front, back, top, bottom, left, right
    };

    Camera(int width, int height, glm::vec3 position);

    Camera& operator=(const Camera& camera);

    void updateMatrix();

    void inputs(GLFWwindow* window);

    void setFov(float fov);
    void setLeftBorder(float value);

    void setNearPlane(float nearPlane);
    void setRightBorder(float value);

    void setFarPlane(float farPlane);
    void setBottomBorder(float value);

    void setTopBorder(float value);

    glm::mat4 getCameraMatrix();

    void activateCamera();

    static Camera* getActiveCamera();

    Plane* getViewFrustum();

    glm::vec3 getOrientation() const;

    void setOrientation(glm::vec3 orientation);

    void setMode(bool isOrthographic);
};

#endif
