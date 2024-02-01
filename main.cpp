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

    GLfloat vertices[] = {
            -1.0f, 0, -1.0f, 0, 0, 0, 1.0f, 0, 0,
            -1.0f, 0, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1, 0,
            1.0f, 0, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1, 0,
            1.0f, 0, 1.0f, 1.0f, 1.0f, 1.0f, 0, 0, 0,
            0, 2.0f, 0, 1.0f, 1.0f, 1.0f, 0, 0.5f, 1.0f
    };

    GLuint indices[] = {
            0, 1, 2,
            3, 2, 1,
            0, 1, 4,
            0, 2, 4,
            2, 3, 4,
            1, 3, 4
    };

    Object object(vertices, sizeof vertices / 9, 9, indices, sizeof indices / 3);
    object.bind();

    //Create shader program
    Shader shader;
    shader.activate("default.frag", "default.vert");
    shader.unloadFiles();

    Camera camera(width, height, glm::vec3(0,0,5.0f));

    //Program scale
    GLuint scaleId = glGetUniformLocation(shader.getProgramID(), "scale");

    //Texture
    Texture texture("Textures/unnamed.jpg");
    GLuint textureId = glGetUniformLocation(shader.getProgramID(), "tex0");

    glEnable(GL_DEPTH_TEST);
    //auto model = glm::vec4(1.0f);

    //Window loop
    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.65f, 0.47f, 0.34f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        object.rotate(glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
        object.bind();

        camera.matrix(45, 0.1f, 100.0f, shader, "camMatrix");
        camera.inputs(window);

        glUniform1f(scaleId, 0.5f);
        glUniform1i(textureId, 0);
        texture.bind();
        object.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
