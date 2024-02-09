#ifndef TRINITYENGINE_SHADER_H
#define TRINITYENGINE_SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "Camera.h"

class Shader {
private:
    GLuint programId = 0, fragmentShader = 0, vertexShader = 0;
    static GLuint activeShader;

    Camera *camera = nullptr;
    bool cameraMovedFlag = false;
    friend void Camera::updateMatrix();

    std::string readFile(std::string location);

public:
    Shader(std::string fragmentShaderSource, std::string vertexShaderSource);

    GLuint getProgramID();

    void activate();

    void unloadFiles();

    void setCamera(Camera &camera);

    ~Shader();
};

#endif
