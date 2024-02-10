#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Structs.h"
#include "SingleTextureMesh.h"
#include "gltfReader.h"
#include "Light.h"

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

    std::vector<Vertex> vertices {
            {{-1.0f, 0, -1.0f},{1, 1, 1},{0, 0, 0},{-1, -0.732f, -1}},
            {{-1.0f, 0, 1.0f},{1, 1, 1},{1, 0, 0},{-1, -0.732f, 1}},
            {{1.0f, 0, -1.0f},{1, 1, 1},{1, 0, 0},{1, -0.732f, -1}},
            {{1.0f, 0, 1.0f},{1, 1, 1},{0, 0, 0},{1, -0.732f, 1}},
            {{0, 2.0f, 0},{1, 1, 1},{0.5f, 1.0f, 0},{0, 1.0f, 0}}
    };

    std::vector<GLuint> indices {
            0, 1, 2,
            3, 1, 2,
            0, 1, 4,
            0, 2, 4,
            3, 1, 4,
            3, 2, 4};

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

    std::vector<GLuint> lightIndices = {
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
            7, 1, 5};

    //Create shader program
    Shader shader("singleTex.frag", "singleTex.vert");
    shader.unloadFiles();

    SingleTextureMesh object(vertices, indices, shader, {"Textures/unnamed.jpg"}, {"Textures/spec.jpg"});
    object.bind();

    Camera camera(width, height, glm::vec3(-0.5f,0.5f,10));

    object.move(glm::vec3(-2));

    Shader lightShader("light.frag", "light.vert");
    lightShader.unloadFiles();

    SingleTextureMesh object2 = object;
    object2.bind();

    gltfReader chess("test/ABeautifulGame.gltf", shader);
    Scene scene = chess.getScene(0);

    Light light(lightVertices, lightIndices, lightShader, glm::vec3(1), glm::vec3(-1), LightingType::PointLight);
    light.bind();
    light.move(glm::vec3(5));

    Light light2(lightVertices, lightIndices, lightShader, glm::vec3(1, 0, 0), glm::vec3(-1), LightingType::PointLight);
    light2.bind();
    light2.move(glm::vec3(-10));

    glEnable(GL_DEPTH_TEST);
    double prevTime = glfwGetTime();
    object.setAngularVelocity(glm::vec3(0, 3, 0));
    shader.addLight(light);
    shader.addLight(light2);

    light.setScale(glm::vec3(0.2f));
    light2.setScale(glm::vec3(0.25f));
    object.setScale(glm::vec3(1, 10, 1));

    shader.setCamera(camera);
    lightShader.setCamera(camera);
    camera.addShader(shader);
    camera.addShader(lightShader);
    camera.setFov(45);
    camera.setNearPlane(0.1f);
    camera.setFarPlane(100);

    double timer = 0;
    double blinker = 0;
    double blinkCnt = 0;
    bool blink = false;
    bool isOn = true;

    glfwSetCursorPos(window, width/2, height/2);

    //Window loop
    while(!glfwWindowShouldClose(window)) {
        double currentTIme = glfwGetTime();
        double timeDelta = currentTIme - prevTime;
        glClearColor(0.65f, 0.47f, 0.34f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(!blink) timer += timeDelta;
        if(timer > 2) {
            timer = 0;
            blink = true;
        }
        if(blink) blinker += timeDelta;
        if(blinker > 0.1) {
            blinker = 0;
            if(isOn) light2.setColor(glm::vec3(0));
            else light2.setColor(glm::vec3(1, 0, 0));
            isOn = !isOn;
            if(++blinkCnt > 8) {
                blinkCnt = 0;
                blink = false;
            }
        }

        object.update(timeDelta);
        scene.draw();
        //object.draw();
        //object2.draw();

        lightShader.activate();
        light2.draw();
        light.draw();

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
