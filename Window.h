#ifndef TRINITYENGINE_WINDOW_H
#define TRINITYENGINE_WINDOW_H

#include "Shader.h"
#include "Text.h"
#include "gltfReader.h"
#include "Camera.h"

class Window {
private:
    GLFWwindow *window;
    int width;
    int height;

    Camera camera;

    Shader *drawShader = nullptr;
    Shader *shadowShader = nullptr;
    Shader *textShader = nullptr;
    const unsigned char DRAW_SHADER_BIT = 0b1;
    const unsigned char SHADOW_SHADER_BIT = 0b10;
    const unsigned char TEXT_SHADER_BIT = 0b100;
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

    static constexpr int maxLightCount = 16;
    Light* lights[maxLightCount];
    unsigned long long lightEnabled = -1;
    int lightCount = 0;
    bool lightsChanged = false;
    void loadLights();
    float shadowSampleCount = 1 / 24.0;

public:
    explicit Window(int width = 1280, int height = 720); //Initialize window

    GLFWwindow* getID(); //Get window id

    void setDrawShader(Shader &shader); //Set given shader as draw shader
    void setDrawShader(const std::string& uri); //Load shader from given location(WITHOUT extensions) and set it as draw shader (Loads: .vert, .frag, .geom)

    void setShadowShader(Shader &shader); //Set given shader as shadow shader
    void setShadowShader(const std::string& uri); //Load shader from given location(WITHOUT extensions) and set it as shadow shader (Loads: .vert, .frag, .geom)

    void setTextShader(Shader &shader); //Set given shader as text shader
    void setTextShader(const std::string& uri); //Load shader from given location(WITHOUT extensions) and set it as text shader (Loads: .vert, .frag, .geom)

    const Camera& getCamera() const; //Get constant reference to window camera
    Camera& getCamera(); //Get mutable reference to window camera

    [[nodiscard]] int addText(std::string font, bool fixed = true); /*Add text to window with given parameters
 *Returns text index
 *Fixed and non-fixed texts have separate indices*/
    Text& getText(int index, bool fixed); //Get text with given index and fixed state
    [[nodiscard]] int copyText(int index, bool fixed); //Copy text with given index and get the index of the copy

    void loadGLTF(std::string location); //Load gltf file
    [[nodiscard]] int getScene(int index); //Get scene from currently loaded gltf file
    int getSceneCount(); //Get scene count of currently loaded gltf file
    void unloadGLTF(); //Unload gltf file

    Model& getModel(int index); //Get model at given index
    [[nodiscard]] int copyModel(int index); //Copy model with the given index and get the index of the copied model

    [[nodiscard]] int addLight(glm::vec3 color, glm::vec3 direction, LightingType type, unsigned int shadowWidth = 1024, unsigned int shadowHeight = 1024); //Add light with given parameters and get its index
    Light& getLight(int index); //Get light at given index

    float getTimeDelta(); //Get the time which elapsed between frame renderings

    void drawFrame(); //Render frame

    void setShadowSampleRate(unsigned int samplesPerSecond = 0); //Set the maximum number of times the shadows should be sampled per second, if no argument is given it renders shadows every frame

    void enableLight(int index); //Enable light at given index
    void disableLight(int index); //Disable light at given index
    bool isLightActive(int index) const; //Check if the light at the given index is enabled

    ~Window();
};

#endif
