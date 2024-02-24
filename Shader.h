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

    std::string readFile(std::string location);

public:
    Shader(std::string fragmentShaderSource, std::string vertexShaderSource);

    GLuint getProgramID();

    void activate();

    void unloadFiles();

    ~Shader();

    static GLuint getActiveShader();
};

#endif
