#include "Window.h"
#include "TextureHandler.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <utility>
#include "Model.h"
#include "Light.h"

Window::Window(int width, int height) : width(width), height(height), camera(width, height, glm::vec3(0)) {
    glfwInit();

    //Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Initalize window and handle case where it doesn't open
    window = glfwCreateWindow(width, height, "Test", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
}

void Window::setDrawShader(Shader &shader) {
    if(dynamicShader & DRAW_SHADER_BIT) delete drawShader;
    drawShader = &shader;
    dynamicShader &= ~DRAW_SHADER_BIT;
    drawShader->setCamera(camera);
    camera.addShader(*drawShader);
}

void Window::setShadowShader(Shader &shader) {
    if(dynamicShader & SHADOW_SHADER_BIT) delete shadowShader;
    shadowShader = &shader;
    dynamicShader &= ~SHADOW_SHADER_BIT;
}

void Window::setTextShader(Shader &shader) {
    if(dynamicShader & TEXT_SHADER_BIT) delete textShader;
    textShader = &shader;
    dynamicShader &= ~TEXT_SHADER_BIT;
}

void Window::setDrawShader(const std::string& uri) {
    if(dynamicShader & DRAW_SHADER_BIT) delete drawShader;
    drawShader = new Shader(uri + ".frag", uri + ".vert");
    drawShader->unloadFiles();
    dynamicShader |= DRAW_SHADER_BIT;
    drawShader->setCamera(camera);
    camera.addShader(*drawShader);
}

void Window::setShadowShader(std::string uri) {
    if(dynamicShader & SHADOW_SHADER_BIT) delete shadowShader;
    shadowShader = new Shader(uri + ".frag", uri + ".vert");
    shadowShader->unloadFiles();
    dynamicShader |= SHADOW_SHADER_BIT;
}

void Window::setTextShader(std::string uri) {
    if(dynamicShader & TEXT_SHADER_BIT) delete textShader;
    textShader = new Shader(uri + ".frag", uri + ".vert");
    textShader->unloadFiles();
    dynamicShader |= TEXT_SHADER_BIT;
}

Window::~Window() {
    TextureHandler::killTextureHandler();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::setFov(float fov) {
    camera.setFov(fov);
}

void Window::setNearPlane(float nearPlane) {
    camera.setNearPlane(nearPlane);
}

void Window::setFarPlane(float farPlane) {
    camera.setFarPlane(farPlane);
}

void Window::setCamera(Camera &camera) {
    this->camera = camera;
}

int Window::addText(std::string font, bool fixed) {
    if(fixed) {
        texts.push_back(new Text(std::move(font), width, height, fixed));
        return texts.size() - 1;
    }
    else {
        textMeshes.push_back(new Text(std::move(font), width, height, fixed));
        return textMeshes.size() - 1;
    }
}

Text &Window::getText(int index, bool fixed) {
    if(fixed) return *texts.at(index);
    else return *textMeshes.at(index);
}

float Window::getTimeDelta() {
    return timeDelta;
}

GLFWwindow* Window::getID() {
    return window;
}

void Window::loadGLTF(std::string location) {
    delete gltf;
    gltf = new gltfReader(std::move(location));
}

void Window::unloadGLTF() {
    delete gltf;
}

int Window::getScene(int index) {
    if(!gltf) throw std::range_error("Cannot get a scene, because the no gltf file is loaded.");
    models.push_back(&gltf->getScene(index));
    return models.size() - 1;
}

int Window::getSceneCount() {
    if(!gltf) throw std::range_error("Cannot get the scene count, because the no gltf file is loaded.");
    return gltf->getSceneCount();
}

Model &Window::getModel(int index) {
    return *models.at(index);
}

int Window::copyModel(int index) {
    models.push_back(new Model(*models.at(index)));
    return models.size() - 1;
}

int Window::copyText(int index, bool fixed) {
    if(fixed) {
        texts.push_back(new Text(*texts.at(index)));
        return texts.size() - 1;
    }
    else {
        textMeshes.push_back(new Text(*textMeshes.at(index)));
        return textMeshes.size() - 1;
    }
}

Light &Window::getLight(int index) {
    if(!drawShader) throw std::range_error("Cannot get light, because the draw shader has not been set yet.");
    return drawShader->getLight(index);
}

int Window::addLight(glm::vec3 color, glm::vec3 direction, LightingType type) {
    if(!drawShader) throw std::range_error("Cannot add light, because the draw shader has not been set yet.");
    drawShader->addLight(*new Light(color, direction, type));
    return drawShader->getLightCount() - 1;
}

void Window::render(bool loadTextures) {
    for(Model* model : models) model->draw(loadTextures);
    for(Text *text : textMeshes) text->draw(loadTextures);
}

void Window::drawFrame() {
    TextureHandler::getTextureHandler().assignTexture();
    float currentTime = glfwGetTime();
    timeDelta = currentTime - previousTime;
    previousTime = currentTime;

    camera.inputs(window);
    camera.update(timeDelta);
    updateModels(timeDelta);

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.65f, 0.47f, 0.34f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawShader->activate();
    render(true);
    textShader->activate();
    for(Text *text : texts) text->draw(false);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::updateModels(float timeDelta) {
    for(Model* model : models) model->update(timeDelta);
    for(Text *text : textMeshes) text->getMesh().update(timeDelta);
}

