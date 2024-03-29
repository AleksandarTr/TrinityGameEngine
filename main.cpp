#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "View.h"
#include "Structs.h"
#include "SingleTextureMesh.h"
#include "gltfReader.h"
#include "Light.h"
#include "Model.h"
#include "Text.h"
#include "TextureHandler.h"
#include "Window.h"

int main() {
    Window window;
    window.setDrawShader("singleTex");
    window.setTextShader("text");
    window.setShadowShader("shadow");

    window.loadGLTF("Textures/pyramid.gltf");
    int objectScene = window.getScene(0);
    Model &object = window.getModel(objectScene)[0];
    object.move(glm::vec3(-2));

    window.loadGLTF("Textures/EmissiveStrengthTest.gltf");
    Model &emissionTest = window.getModel(window.getScene(0));
    emissionTest.move(glm::vec3(0, 0, -20));

    window.loadGLTF("test/ABeautifulGame.gltf");
    int sceneIndex = window.getScene(0);
    Model &scene = window.getModel(sceneIndex);
    window.unloadGLTF();
    scene.setAngularVelocity(glm::vec3(0, 0, 1));
    scene[8].setAngularVelocity(glm::vec3(0, -1, 0));
    scene.scale(glm::vec3(5));

    Light& light = window.getLight(window.addLight(glm::vec3(0.5), glm::vec3(0, -1, -3), PointLight));
    light.move(glm::vec3(0, 2, 6));
    light.setFov(90);

    int light2Index = window.addLight(glm::vec3(1, 0, 0), glm::vec3(1), PointLight);
    Light& light2 = window.getLight(light2Index);
    light2.move(glm::vec3(-10));
    light2.setFov(90);

    Light& directionLight = window.getLight(window.addLight(glm::vec3(0.25), glm::vec3(0, -1, -1), DirectionalLight));
    directionLight.move(glm::vec3(0, 5, 5));
    directionLight.setOrthographicBorder(-5, 5, 5, -5);

    Light& spotLight = window.getLight(window.addLight(glm::vec3(0.8), glm::vec3(-1), SpotLight));
    spotLight.move(glm::vec3(5));
    spotLight.setFov(10);

    //object.setAngularVelocity(glm::vec3(0, 3, 0));
    window.setShadowSampleRate(30);

    double timer = 0;
    double blinker = 0;
    double blinkCnt = 0;
    bool blink = false;
    bool isOn = true;

    int textIndex = window.addText("Textures/arial");
    Text& text = window.getText(textIndex, true);
    text.generateMessage("   ", 5, 5, glm::vec3(0, 1, 1));

    int text3DIndex = window.addText("Textures/arial", false);
    Text& text3D = window.getText(text3DIndex, false);
    text3D.generateMessage("great Success, jes!", 5, 5, glm::vec3(1, 0, 0));
    text3D.getMesh().scale(glm::vec3(5));
    text3D.getMesh().setAngularVelocity(glm::vec3(0, 5, 0));
    text3D.getMesh().move(glm::vec3(0, 1, 0));

    float fpsTimer = 0;
    int fps = 0;

    //Window loop
    while(!glfwWindowShouldClose(window.getID())) {
        window.drawFrame();
        float timeDelta = window.getTimeDelta();

        if(!blink) timer += timeDelta;
        if(timer > 2) {
            timer = 0;
            blink = true;
        }
        if(blink) blinker += timeDelta;
        if(blinker > 0.1) {
            blinker = 0;
            if(isOn) window.disableLight(light2Index);
            else window.enableLight(light2Index);
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
            text.setMessage(std::to_string(fps), glm::vec3(0, 1, 1), 5, 5, 50);
            fps = 0;
        }
    }

    return 0;
}
