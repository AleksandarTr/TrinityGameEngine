#ifndef TRINITYENGINE_SHADER_H
#define TRINITYENGINE_SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "Camera.h"

class Light;

class Shader {
private:
    GLuint programId = 0, fragmentShader = 0, vertexShader = 0;
    static GLuint activeShader;

    Camera *camera = nullptr;
    bool cameraMovedFlag = false;
    friend void Camera::updateMatrix();

    std::vector<Light*> lights;
    bool lightsChanged = false;

    std::string readFile(std::string location);

public:
    Shader(std::string fragmentShaderSource, std::string vertexShaderSource);

    GLuint getProgramID();

    void activate();

    void unloadFiles();

    void setCamera(Camera &camera);

    void addLight(Light& light);

    void removeLight(int index = -1);

    void removeLight(Light &light);

    ~Shader();

    void loadLights();
};

#endif
