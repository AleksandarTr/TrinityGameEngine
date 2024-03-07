#ifndef TRINITYENGINE_SHADER_H
#define TRINITYENGINE_SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "View.h"

class Light;

class Shader {
private:
    GLuint programId = 0, fragmentShader = 0, vertexShader = 0, geometryShader = 0;
    static GLuint activeShader;

    std::string readFile(std::string location);

public:
    Shader(std::string fragmentShaderSource, std::string vertexShaderSource, std::string geometryShaderSource = "");

    GLuint getProgramID(); //Get id of the program storing all the separate shaders

    void activate(); //Set this shader as active, used for rending meshes

    void unloadFiles(); //Unload buffers storing the individual shaders

    ~Shader();

    static GLuint getActiveShader(); //Get the currently active shader
};

#endif
