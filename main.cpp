#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Object.h"
#include "Texture.h"
#include "Camera.h"
#include "Light.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

int width = 1280;
int height = 720;

int main() {
    //Initialize GLFW
    glfwInit();

    //Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Initalize window and handle case where it doesn't open
    GLFWwindow *window = glfwCreateWindow(width, height, "Test", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);
    glfwSetCursorPos(window, width/2, height/2);

    GLfloat vertices[] = {
            -1.0f, 0, -1.0f,        0, 0,             0, -1.0f, 0,
            -1.0f, 0, 1.0f,        1, 0,            0, -1.0f, 0,
            1.0f, 0, -1.0f,      0, 1,           0, -1.0f, 0,
            1.0f, 0, 1.0f,       1, 1,           0, -1.0f, 0,

            -1.0f, 0, -1.0f,     0, 0,           -1.0f, 2.0f, 0,
            -1.0f, 0, 1.0f,      1, 0,           -1.0f, 2.0f, 0,
            0, 2.0f, 0,          0.5f, 1.0f,     -1.0f, 2.0f, 0,

            -1.0f, 0, -1.0f,     0, 0,           0, 2.0f, -1.0f,
            1.0f, 0, -1.0f,      1, 0,           0, 2.0f, -1.0f,
            0, 2.0f, 0,          0.5f, 1.0f,    0, 2.0f, -1.0f,

            1.0f, 0, 1.0f,        0, 0,          0, 2.0f, 1.0f,
            -1.0f, 0, 1.0f,       1, 0,          0, 2.0f, 1.0f,
            0, 2.0f, 0,           0.5f, 1.0f,    0, 2.0f, 1.0f,

            1.0f, 0, 1.0f,      0, 0,          1.0f, 2.0f, 0,
            1.0f, 0, -1.0f,     1, 0,          1.0f, 2.0f, 0,
            0, 2.0f, 0,         0.5f, 1.0f,    1.0f, 2.0f, 0,
    };

    GLuint indices[] = {
            0, 1, 2,
            3, 1, 2,
            4, 5, 6,
            7, 8, 9,
            10, 11, 12,
            13, 14, 15
    };

    GLfloat lightVertices[] = {
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
    };

    GLuint lightIndices[] = {
            0, 1, 3,
            0, 2, 3,
            0, 4, 5,
            0, 1, 5,
            0, 4, 6,
            0, 2, 6,
            7, 4, 5,
            7, 4, 6,
            7, 2, 3,
            7, 2, 6,
            7, 1, 3,
            7, 1, 5
    };

    //Create shader program
    Shader shader("default.frag", "default.vert");
    shader.unloadFiles();

    Object object(vertices, sizeof vertices / 8 / sizeof vertices[0], indices, sizeof indices / 3 / sizeof indices[0], shader);
    object.bind();

    Camera camera(width, height, glm::vec3(-0.5f,0.5f,10));

    //Texture
    Texture texture("Textures/unnamed.jpg");

    object.move(glm::vec3(-2));

    Shader lightShader("light.frag", "light.vert");
    lightShader.unloadFiles();

    Light light(lightVertices, sizeof lightVertices / 3 / sizeof(GLfloat), lightIndices, sizeof lightIndices / 3 / sizeof(GLuint), lightShader);
    light.bind();
    light.move(glm::vec3(2));
    glm::vec4 lightColor(1.0f, 1.0f, 1.0f, 1.0f);

    shader.activate();
    glUniform4f(glGetUniformLocation(shader.getProgramID(), "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    object.setLight(light);

    lightShader.activate();
    glUniform4f(glGetUniformLocation(lightShader.getProgramID(), "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

    glEnable(GL_DEPTH_TEST);

    //Window loop
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.65f, 0.47f, 0.34f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.activate();
        object.rotate(glm::vec3(0.0f, 0.001f, 0.0f));

        camera.updateMatrix(45, 0.1f, 100.0f);
        camera.inputs(window);

        camera.setMatrix(shader, "camMatrix");
        glUniform1f(glGetUniformLocation(shader.getProgramID(), "scale"), 1.0f);
        glUniform1i(glGetUniformLocation(shader.getProgramID(), "tex0"), 0);
        texture.bind();
        object.draw();

        lightShader.activate();
        camera.setMatrix(lightShader, "camMatrix");
        glUniform1f(glGetUniformLocation(lightShader.getProgramID(), "scale"), 0.5f);
        light.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
