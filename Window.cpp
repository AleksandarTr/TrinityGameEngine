#include "Window.h"
#include "TextureHandler.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <utility>
#include <stack>
#include "Model.h"
#include "Light.h"

Window::Window(int width, int height) : width(width), height(height), camera(width, height) {
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
    glfwSetCursorPos(window, width/2, height/2);
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
    if(!gltf) throw std::range_error("Cannot get a scene, because no gltf file is loaded.");
    models.push_back(&gltf->getScene(index));

    addModelToDrawOrder(models.at(models.size() - 1));

    return models.size() - 1;
}

int Window::getSceneCount() {
    if(!gltf) throw std::range_error("Cannot get the scene count, because no gltf file is loaded.");
    return gltf->getSceneCount();
}

Model &Window::getModel(int index) {
    return *models.at(index);
}

int Window::copyModel(int index) {
    models.push_back(new Model(*models.at(index)));
    addModelToDrawOrder(models.at(models.size() - 1));
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

int Window::addLight(glm::vec3 color, glm::vec3 direction, LightingType type, unsigned int shadowWidth, unsigned int shadowHeight) {
    if(lightCount >= 16) throw std::out_of_range("Only 16 lights are allowed at any time");
    lights[lightCount] = new Light(color, direction, type, shadowWidth, shadowHeight);
    lightsChanged = true;
    return lightCount++;
}

void Window::render(bool loadTextures, bool drawText) {
    for(Mesh* model : drawOrder) model->draw(loadTextures);
    if(drawText) for(Text *text : textMeshes) text->draw(loadTextures);
}

void Window::drawFrame() {
    if(!drawOrderSorted) sortDrawOrder();

    TextureHandler::assignTexture();
    float currentTime = glfwGetTime();
    timeDelta = currentTime - previousTime;
    previousTime = currentTime;
    updateModels(timeDelta);
    static float shadowCnt = 0;
    shadowCnt += timeDelta;

    if(shadowCnt > shadowSampleCount) {
        shadowCnt = 0;
        shadowShader->activate();
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        unsigned long long check = 1;
        for (int i = 0; i < lightCount; i++) {
            if(!(lightEnabled & check)) {
                check <<= 1;
                continue;
            }
            check <<= 1;

            lights[i]->drawShadowMap();
            render(false, false);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);

    glClearColor(0.65f, 0.47f, 0.34f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.update(timeDelta);
    camera.controls(window);
    camera.activate();
    TextureHandler::resetActiveTextures();

    drawShader->activate();
    loadLights();
    glUniformMatrix4fv(glGetUniformLocation(drawShader->getProgramID(), "camMatrix"), 1, GL_FALSE,glm::value_ptr(camera.getCameraMatrix()));
    render(true, true);
    textShader->activate();

    glDisable(GL_DEPTH_TEST);
    for(Text *text : texts) text->draw(false);
    glEnable(GL_DEPTH_TEST);


    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Window::updateModels(float timeDelta) {
    for(Model* model : models) model->update(timeDelta);
    for(Text *text : textMeshes) text->getMesh().update(timeDelta);
}

void Window::loadLights() {
    int offset = 0;
    unsigned long long check = 1;
    for(int i = 0; i < lightCount; i++) {
        if(!(lightEnabled & check)) {
            check <<= 1;
            offset++;
            continue;
        }
        check <<= 1;

        lights[i]->loadLight(i - offset);
        lights[i]->getShadowMap().getInfo().type = static_cast<TextureType>(TE_ShadowMap0 + i - offset);
        TextureHandler::bindTexture(lights[i]->getShadowMap());
    }

    glUniform1i(glGetUniformLocation(drawShader->getProgramID(), "lightNum"), lightCount - offset);
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

void Window::setShadowSampleRate(int samplesPerSecond) {
    shadowSampleCount = 1.0f / samplesPerSecond;
}

void Window::addModelToDrawOrder(Model *newModel) {
    std::stack<Model*> s;
    s.push(newModel);

    while(!s.empty()) {
        Model *model = s.top();
        s.pop();

        for(int i = 0; i < model->getMeshCount(); i++) drawOrder.push_back(model->getMesh(i));
        for(int i = 0; i < model->getModelCount(); i++) s.push(&(*model)[i]);
    }

    drawOrderSorted = false;
}

void Window::sortDrawOrder() {
    drawOrderSorted = true;
    int lastSuitablePosition;
    int suitability;
    static constexpr TextureType textureTypes[] = {DiffuseTexture, PBRTexture, NormalTextre, OcclusionTexture, EmissiveTexture};
    static constexpr int textureTypeCount = sizeof textureTypes / sizeof textureTypes[0];

    for(int i = 0; i < drawOrder.size(); i++)
    {
        lastSuitablePosition = i;
        suitability = 0;
        auto &currentMesh = dynamic_cast<SingleTextureMesh&>(*drawOrder.at(i));
        for(int j =  i - 1; j >= 0; j--) {
            auto &prevMesh = dynamic_cast<SingleTextureMesh&>(*drawOrder.at(j));
            int currentSuitability = 0;

            for(auto textureType : textureTypes) {
                if(!currentMesh.getTexture(textureType)) {
                    if(!prevMesh.getTexture(textureType)) currentSuitability++;
                    continue;
                }
                if (!prevMesh.getTexture(textureType) ||
                    currentMesh.getTexture(textureType)->getInfo().location ==
                    prevMesh.getTexture(textureType)->getInfo().location)
                    currentSuitability++;
            }

            if(currentSuitability > suitability) {
                lastSuitablePosition = j;
                suitability = currentSuitability;
            }
            if(currentSuitability == textureTypeCount) break;
        }

        for(int j = i; j > lastSuitablePosition; j--) drawOrder.at(j) = drawOrder.at(j-1);
        drawOrder.at(lastSuitablePosition) = &currentMesh;
    }
}

const Camera &Window::getCamera() const {
    return camera;
}

Camera &Window::getCamera() {
    return camera;
}

void Window::enableLight(int index) {
    if(index < 0 || index > maxLightCount) return;
    lightEnabled |= 1 << index;
}

void Window::disableLight(int index) {
    if(index < 0 || index > maxLightCount) return;
    lightEnabled &= ~(1 << index);
}

bool Window::isLightActive(int index) const {
    if(index < 0 || index > maxLightCount) return false;
    return lightEnabled & 1 << index;
}
