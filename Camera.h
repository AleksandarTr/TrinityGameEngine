#ifndef TRINITYENGINE_CAMERA_H
#define TRINITYENGINE_CAMERA_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include "Shader.h"
#include "MoveableObject.h"

class Camera : public MoveableObject {
private:
    glm::vec3 orientation = glm::vec3(0,0,-1.0f);
    glm::vec3 up = glm::vec3(0, 1.0f, 0);
    glm::mat4 cameraMat;

    int width;
    int height;

    float sensitivity = 500.0f;
    float speed = 10.0f;

public:
    Camera(int width, int height, glm::vec3 position);

    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);

    void setMatrix(Shader &shader, const char *uniform);

    void inputs(GLFWwindow* window);
};

#endif
