#ifndef TRINITYENGINE_SHADER_H
#define TRINITYENGINE_SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>

class Shader {
private:
    GLuint programId, fragmentShader, vertexShader;

    std::string readFile(std::string location);

public:
    Shader(std::string fragmentShaderSource, std::string vertexShaderSource);

    GLuint getProgramID();

    void activate();

    void unloadFiles();

    ~Shader();
};

#endif
