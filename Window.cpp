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
    changeShader(drawShader, shader, DRAW_SHADER_BIT);
}

void Window::setShadowShader(Shader &shader) {
    changeShader(shadowShader, shader, SHADOW_SHADER_BIT);
}

void Window::setTextShader(Shader &shader) {
    changeShader(textShader, shader, TEXT_SHADER_BIT);
}

void Window::setDrawShader(const std::string& uri) {
    loadShader(drawShader, uri, DRAW_SHADER_BIT);
}

void Window::setShadowShader(const std::string& uri) {
    loadShader(shadowShader, uri, SHADOW_SHADER_BIT);
}

void Window::setTextShader(const std::string& uri) {
    loadShader(textShader, uri, TEXT_SHADER_BIT);
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
    if(index < 0 || index >= lightCount) throw std::out_of_range("Index is out of range of valid lights");
    return *lights[index];
}

int Window::addLight(glm::vec3 color, glm::vec3 direction, LightingType type) {
    if(lightCount >= 16) throw std::out_of_range("Only 16 lights are allowed at any time");
    lights[lightCount] = new Light(color, direction, type);
    lightsChanged = true;
    return lightCount++;
}

void Window::render(bool loadTextures, bool drawText) {
    for(Model* model : models) model->draw(loadTextures);
    if(drawText) for(Text *text : textMeshes) text->draw(loadTextures);
}

void Window::drawFrame() {
    TextureHandler::getTextureHandler().assignTexture();
    float currentTime = glfwGetTime();
    timeDelta = currentTime - previousTime;
    previousTime = currentTime;
    updateModels(timeDelta);
    static float shadowCnt = 0;
    shadowCnt += timeDelta;

    glEnable(GL_CULL_FACE);
    if(shadowCnt > shadowSampleCount) {
        shadowCnt = 0;
        shadowShader->activate();
        glCullFace(GL_FRONT);

        glViewport(0, 0, Light::shadowWidth, Light::shadowHeight);
        for (int i = 0; i < lightCount; i++) {
            lights[i]->drawShadowMap();
            render(false, false);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glCullFace(GL_BACK);

    glClearColor(0.65f, 0.47f, 0.34f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.inputs(window);
    camera.update(timeDelta);

    drawShader->activate();
    loadLights();
    glUniformMatrix4fv(glGetUniformLocation(drawShader->getProgramID(), "camMatrix"), 1, GL_FALSE,glm::value_ptr(camera.getCameraMatrix()));
    render(true, true);
    textShader->activate();
    for(Text *text : texts) text->draw(false);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::updateModels(float timeDelta) {
    for(Model* model : models) model->update(timeDelta);
    for(Text *text : textMeshes) text->getMesh().update(timeDelta);
}

void Window::loadLights() {
    for(int i = 0; i < lightCount; i++) {
        glm::vec3 lightSource = lights[i]->getPosition();
        glm::vec3 lightDir = lights[i]->getDirection();
        glm::vec3 lightColor = lights[i]->getColor();

        char fieldName[20];
        sprintf(fieldName, "lightPos[%d]", i);
        glUniform3f(glGetUniformLocation(drawShader->getProgramID(), fieldName), lightSource.x, lightSource.y,lightSource.z);

        sprintf(fieldName, "lightingType[%d]", i);
        glUniform1i(glGetUniformLocation(drawShader->getProgramID(), fieldName),static_cast<GLint>(lights[i]->getType()));

        sprintf(fieldName, "lightDir[%d]", i);
        glUniform3f(glGetUniformLocation(drawShader->getProgramID(), fieldName), lightDir.x, lightDir.y, lightDir.z);

        sprintf(fieldName, "lightColor[%d]", i);
        glUniform4f(glGetUniformLocation(drawShader->getProgramID(), fieldName), lightColor.x, lightColor.y, lightColor.z,1.0f);

        lights[i]->getShadowMap().getInfo().type = static_cast<TextureType>(static_cast<int>(TextureType::ShadowMap0) + i);
        TextureHandler::bindTexture(lights[i]->getShadowMap());
        sprintf(fieldName, "lightMatrix[%d]", i);
        glUniformMatrix4fv(glGetUniformLocation(drawShader->getProgramID(), fieldName), 1, false, glm::value_ptr(lights[i]->getLightMatrix()));

        sprintf(fieldName, "shadowMap[%d]", i);
        glUniform1i(glGetUniformLocation(drawShader->getProgramID(), fieldName),static_cast<GLint>(lights[i]->getShadowMap().getInfo().type));
    }
    glUniform1i(glGetUniformLocation(drawShader->getProgramID(), "lightNum"), lightCount);
    lightsChanged = false;
}

void Window::loadShader(Shader *&shader, const std::string &uri, unsigned char shaderBit) {
    if(dynamicShader & shaderBit) delete shader;
    bool geometryShader = false;
    if(FILE *check = fopen(std::string(uri + ".geom").c_str(), "r")) {
        geometryShader = true;
        fclose(check);
    }

    shader = new Shader(uri + ".frag", uri + ".vert", geometryShader ? uri + ".geom" : "");
    shader->unloadFiles();
    dynamicShader |= shaderBit;
}

void Window::changeShader(Shader *&localShader, Shader &externalShader, unsigned char shaderBit) {
    if(dynamicShader & shaderBit) delete localShader;
    localShader = &externalShader;
    dynamicShader &= ~shaderBit;
}
