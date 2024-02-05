#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "Structs.h"

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

    std::vector<Vertex> vertices {
            {{-1.0f, 0, -1.0f},{1, 1, 1},{0, 0, 0},{-1, -0.732f, -1}},
            {{-1.0f, 0, 1.0f},{1, 1, 1},{1, 0, 0},{-1, -0.732f, 1}},
            {{1.0f, 0, -1.0f},{1, 1, 1},{1, 0, 0},{1, -0.732f, -1}},
            {{1.0f, 0, 1.0f},{1, 1, 1},{0, 0, 0},{1, -0.732f, 1}},
            {{0, 2.0f, 0},{1, 1, 1},{0.5f, 1.0f, 0},{0, 1.0f, 0}}
    };

    std::vector<Index> indices {
            {{0, 1, 2}},
            {{3, 1, 2}},
            {{0, 1, 4}},
            {{0, 2, 4}},
            {{3, 1, 4}},
            {{3, 2, 4}}};

    std::vector<Vertex> lightVertices = {
            {{-1.0f, -1.0f, -1.0f}, {1, 1, 1}, {}, {}},
            {{-1.0f, -1.0f, 1.0f}, {1, 1, 1}, {}, {}},
            {{-1.0f, 1.0f, -1.0f}, {1, 1, 1}, {}, {}},
            {{-1.0f, 1.0f, 1.0f}, {1, 1, 1}, {}, {}},
            {{1.0f, -1.0f, -1.0f}, {1, 1, 1}, {}, {}},
            {{1.0f, -1.0f, 1.0f}, {1, 1, 1}, {}, {}},
            {{1.0f, 1.0f, -1.0f}, {1, 1, 1}, {}, {}},
            {{1.0f, 1.0f, 1.0f}, {1, 1, 1}, {}, {}}
    };

    std::vector<Index> lightIndices = {
            {{0, 1, 3}},
            {{0, 2, 3}},
            {{0, 4, 5}},
            {{0, 1, 5}},
            {{0, 4, 6}},
            {{0, 2, 6}},
            {{7, 4, 5}},
            {{7, 4, 6}},
            {{7, 2, 3}},
            {{7, 2, 6}},
            {{7, 1, 3}},
            {{7, 1, 5}}
    };

    //Create shader program
    Shader shader("default.frag", "default.vert");
    shader.unloadFiles();

    Object object(vertices, indices, shader, {"Textures/unnamed.jpg"}, {"Textures/spec.jpg"});
    object.bind();

    Camera camera(width, height, glm::vec3(-0.5f,0.5f,10));

    object.move(glm::vec3(-2));

    Shader lightShader("light.frag", "light.vert");
    lightShader.unloadFiles();

    Light light(lightVertices, lightIndices, lightShader, glm::vec3(1), glm::vec3(-1), LightingType::PointLight);
    light.bind();
    light.move(glm::vec3(5));

    Light light2(lightVertices, lightIndices, lightShader, glm::vec3(1), glm::vec3(-1), LightingType::PointLight);
    light2.bind();
    light2.move(glm::vec3(-10));

    glEnable(GL_DEPTH_TEST);
    double prevTime = glfwGetTime();
    object.setAngularVelocity(glm::vec3(0, 3, 0));
    object.setCamera(camera);
    light.setCamera(camera);
    light2.setCamera(camera);
    object.addLight(light);
    object.addLight(light2);

    light.setScale(0.2f);
    light2.setScale(0.25f);
    object.setScale(3);

    double timer = 0;
    double blinker = 0;
    double blinkCnt = 0;
    bool blink = false;
    bool isOn = true;

    //Window loop
    while(!glfwWindowShouldClose(window)) {
        double currentTIme = glfwGetTime();
        double timeDelta = currentTIme - prevTime;
        glClearColor(0.65f, 0.47f, 0.34f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.activate();
        if(!blink) timer += timeDelta;
        if(timer > 2) {
            timer = 0;
            blink = true;
        }
        if(blink) blinker += timeDelta;
        if(blinker > 0.1) {
            blinker = 0;
            if(isOn) light2.setColor(glm::vec3(0));
            else light2.setColor(glm::vec3(1));
            isOn = !isOn;
            if(++blinkCnt > 10) {
                blinkCnt = 0;
                blink = false;
            }
        }

        camera.updateMatrix(45, 0.1f, 100.0f);
        object.update(timeDelta);
        object.draw();

        lightShader.activate();
        light.draw();
        light2.draw();

        camera.inputs(window);
        camera.update(timeDelta);

        prevTime = currentTIme;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
