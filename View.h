#ifndef TRINITYENGINE_VIEW_H
#define TRINITYENGINE_VIEW_H

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

class View : public Movable {
public:
    struct Plane {
        glm::vec3 normal;
        float d;
    };

    enum FrustumSide {
        FrustumFront, FrustumBack, FrustumTop, FrustumBottom, FrustumLeft, FrustumRight
    };

private:
    static View *activeView;

    glm::vec3 orientation = glm::vec3(1,0,0);
    glm::vec3 up = glm::vec3(0, 1.0f, 0);
    glm::mat4 cameraMat = glm::mat4(1);

    const int width;
    const int height;

    bool orthographic;
    float nearPlane;
    float farPlane;
    union {
        float fov;
        struct {
            float left;
            float right;
            float bottom;
            float top;
        };
    };

    Plane frustum[6];
    void calculateCameraFrustum();

public:
    View(int width, int height, float fov, float nearPlane, float farPlane);

    View(int width, int height, float left, float right, float bottom, float top, float nearPlane, float farPlane);

    View& operator=(const View& camera);

    void updateMatrix();

    void setFov(float fov);
    float getFov() const;

    void setNearPlane(float nearPlane);
    float getNearPlane() const;

    void setFarPlane(float farPlane);
    float getFarPlane() const;

    void setLeftBorder(float value);
    void setRightBorder(float value);
    void setBottomBorder(float value);
    void setTopBorder(float value);
    void setOrthographicBorder(float left, float right, float bottom, float top);

    glm::mat4 getCameraMatrix() const;

    void activate();
    static View* getActiveView();

    const Plane* getViewFrustum() const;

    void setOrientation(glm::vec3 orientation);
    glm::vec3 getOrientation() const;

    void setUp(glm::vec3 value);
    glm::vec3 getUp() const;

    float getWidth() const;
    float getHeight() const;
};

#endif
