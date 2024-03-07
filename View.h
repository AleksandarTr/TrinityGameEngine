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

    const unsigned int width;
    const unsigned int height;

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
    View(unsigned int width, unsigned int height, float fov, float nearPlane, float farPlane); //Initialize view with perspective projection with the given parameters

    View(unsigned int width, unsigned int height, float left, float right, float bottom, float top, float nearPlane, float farPlane); //Initialize view with orthogonal projection with given parameters

    View& operator=(const View& camera); //Copy view

    void updateMatrix(); //Update projection and view matrix product

    void setFov(float fov); //Set fov and implicitly change the view to perspective projection
    float getFov() const; //Get fov regardless of projection type

    void setNearPlane(float nearPlane); //Set near plane value (the closest an object can be to the view to be visible)
    float getNearPlane() const; //Get the near plane value

    void setFarPlane(float farPlane); //Set far plane value (the farthest an object can be from the view to be visible)
    float getFarPlane() const; //Get the far plane value

    void setLeftBorder(float value); //Set left border and implicitly change the view to orthogonal projection
    void setRightBorder(float value); //Set right border and implicitly change the view to orthogonal projection
    void setBottomBorder(float value); //Set bottom border and implicitly change the view to orthogonal projection
    void setTopBorder(float value); //Set top border and implicitly change the view to orthogonal projection
    void setOrthographicBorder(float left, float right, float bottom, float top); //Set all borders and implicitly change the view to orthogonal projection

    glm::mat4 getCameraMatrix() const; //Get the calculated view matrix

    void activate(); //Set this view as the active one when rendering
    static View* getActiveView(); //Get the current active view

    const Plane* getViewFrustum() const; //Get a pointer to the view frustum

    void setOrientation(glm::vec3 orientation); //Set where the view is pointing towards
    glm::vec3 getOrientation() const; //Get where the view is pointing towards

    void setUp(glm::vec3 value); //Set where up is
    glm::vec3 getUp() const; //Get where up is

    float getWidth() const; //Get width of the view
    float getHeight() const; //Get height of the view
};

#endif
