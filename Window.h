#ifndef TRINITYENGINE_WINDOW_H
#define TRINITYENGINE_WINDOW_H

#include "Shader.h"
#include "Text.h"
#include "gltfReader.h"

class Window {
private:
    GLFWwindow *window;
    int width;
    int height;

    Camera camera;

    Shader *drawShader = nullptr;
    Shader *shadowShader = nullptr;
    Shader *textShader = nullptr;
    const unsigned char DRAW_SHADER_BIT = 0b00000001;
    const unsigned char SHADOW_SHADER_BIT = 0b0000001;
    const unsigned char TEXT_SHADER_BIT = 0b000001;
    unsigned char dynamicShader = 0;
    void loadShader(Shader*&, const std::string&, unsigned char);
    void changeShader(Shader*&, Shader&, unsigned char);

    std::vector<Model*> models;
    std::vector<Text*> textMeshes;
    std::vector<Text*> texts;

    std::vector<Mesh*> drawOrder;
    bool drawOrderSorted = false;
    void addModelToDrawOrder(Model*);
    void sortDrawOrder();

    gltfReader *gltf = nullptr;

    float previousTime = 0, timeDelta = 0;
    void render(bool loadTextures, bool drawText);
    void updateModels(float timeDelta);

    Light* lights[16];
    int lightCount = 0;
    bool lightsChanged = false;
    void loadLights();
    float shadowSampleCount = 1 / 24.0;

public:
    explicit Window(int width = 1280, int height = 720);

    GLFWwindow* getID();

    void setDrawShader(Shader &shader);
    void setDrawShader(const std::string& uri);

    void setShadowShader(Shader &shader);
    void setShadowShader(const std::string& uri);

    void setTextShader(Shader &shader);
    void setTextShader(const std::string& uri);

    void setFov(float fov);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);
    void setCamera(Camera& camera);

    [[nodiscard]] int addText(std::string font, bool fixed = true);
    Text& getText(int index, bool fixed);
    [[nodiscard]] int copyText(int index, bool fixed);

    void loadGLTF(std::string location);
    [[nodiscard]] int getScene(int index);
    int getSceneCount();
    void unloadGLTF();

    Model& getModel(int index);
    [[nodiscard]] int copyModel(int index);

    [[nodiscard]] int addLight(glm::vec3 color, glm::vec3 direction, LightingType type);
    Light& getLight(int index);

    float getTimeDelta();

    void drawFrame();

    void setShadowSampleRate(int samplesPerSecond);

    ~Window();
};

#endif
