#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Structs.h"
#include "SingleTextureMesh.h"
#include "gltfReader.h"
#include "Light.h"
#include "Model.h"
#include "Text.h"
#include "TextureHandler.h"

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

    //Create shader program
    Shader shader("singleTex.frag", "singleTex.vert");
    shader.unloadFiles();

    gltfReader objectReader("Textures/pyramid.gltf", shader);
    Model &objectScene = objectReader.getScene(0);
    Model &object = objectScene[0];

    Camera camera(width, height, glm::vec3(-0.5f,0.5f,10));

    object.move(glm::vec3(-2));

    Model object2 = object;

    auto chess = new gltfReader("test/ABeautifulGame.gltf", shader);
    Model &scene = chess->getScene(0);
    delete chess;
    scene.setAngularVelocity(glm::vec3(0, 2, 0));

    Light light(glm::vec3(1), glm::vec3(-1), LightingType::PointLight);
    light.move(glm::vec3(5));

    Light light2(glm::vec3(1, 0, 0), glm::vec3(-1), LightingType::PointLight);
    light2.move(glm::vec3(-10));

    double prevTime = glfwGetTime();
    object.setAngularVelocity(glm::vec3(0, 3, 0));
    shader.addLight(light);
    //shader.addLight(light2);

    light.setScale(glm::vec3(0.2f));
    light2.setScale(glm::vec3(0.25f));
    object.setScale(glm::vec3(1, 10, 1));
    scene.scale(glm::vec3(10));

    shader.setCamera(camera);
    camera.addShader(shader);
    camera.setFov(45);
    camera.setNearPlane(0.1f);
    camera.setFarPlane(100);

    double timer = 0;
    double blinker = 0;
    double blinkCnt = 0;
    bool blink = false;
    bool isOn = true;

    glfwSetCursorPos(window, width/2, height/2);

    Shader textShader("text.frag", "text.vert");
    textShader.unloadFiles();

    Text text("Textures/arial", textShader, width, height);
    text.generateMessage("   ", 5, 5, glm::vec3(0, 1, 1));

    Text text3D("Textures/arial", shader, width, height, false);
    text3D.generateMessage("Hello world!", 5, 5, glm::vec3(1, 0, 0));
    text3D.getMesh().scale(glm::vec3(5));
    text3D.getMesh().setAngularVelocity(glm::vec3(0, 5, 0));

    float fpsTimer = 0;
    int fps = 0;

    TextureHandler& textureHandler = TextureHandler::getTextureHandler();

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    camera.inputs(window);

    //Window loop
    while(!glfwWindowShouldClose(window)) {
        double currentTIme = glfwGetTime();
        double timeDelta = currentTIme - prevTime;
        glClearColor(0.65f, 0.47f, 0.34f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        textureHandler.assignTexture();

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

        fpsTimer += timeDelta;
        fps++;
        if(fpsTimer > 1) {
            fpsTimer = 0;
            fps %= 1000;
            text.setMessage(std::to_string(fps), glm::vec3(0, 1, 1));
            fps = 0;
        }

        object.update(timeDelta);
        scene.update(timeDelta);
        text3D.getMesh().update(timeDelta);
        scene.draw();
        object.draw();
        text3D.draw();

        text.draw();

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
