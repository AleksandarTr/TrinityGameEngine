#include "Shader.h"
#include <algorithm>
#include "Light.h"

GLuint Shader::activeShader = 0;

Shader::Shader(std::string fragmentShaderSource, std::string vertexShaderSource) {
    programId = glCreateProgram();

    std::string fragmentShaderStr = readFile(fragmentShaderSource);
    std::string vertexShaderStr = readFile(vertexShaderSource);
    const char* fragmentShader = fragmentShaderStr.c_str();
    const char* vertexShader = vertexShaderStr.c_str();

    this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vertexShader, 1, &vertexShader, NULL);
    glCompileShader(this->vertexShader);

    this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fragmentShader, 1, &fragmentShader, NULL);
    glCompileShader(this->fragmentShader);

    //Create shader program
    glAttachShader(programId, this->vertexShader);
    glAttachShader(programId, this->fragmentShader);
    glLinkProgram(programId);
}

std::string Shader::readFile(std::string location) {
    std::ifstream reader(location);
    if(!reader.is_open()) throw std::invalid_argument("Could not open given file");

    std::string result = "";
    std::string line;
    while(std::getline(reader, line)) result += line + "\n";

    reader.close();
    return result;
}

void Shader::loadLights(){
    for(int i = 0; i < lights.size(); i++) {
        glm::vec3 lightSource = lights[i]->getPosition();
        glm::vec3 lightDir = lights[i]->getDirection();
        glm::vec3 lightColor = lights[i]->getColor();

        char l1[20];
        char l2[20];
        char l3[20];
        char l4[20];
        sprintf(l1, "lightPos[%d]", i);
        sprintf(l2, "lightingType[%d]", i);
        sprintf(l3, "lightDir[%d]", i);
        sprintf(l4, "lightColor[%d]", i);

        glUniform3f(glGetUniformLocation(programId, l1), lightSource.x, lightSource.y,lightSource.z);
        glUniform1i(glGetUniformLocation(programId, l2),static_cast<GLint>(lights[i]->getType()));
        glUniform3f(glGetUniformLocation(programId, l3), lightDir.x, lightDir.y, lightDir.z);
        glUniform4f(glGetUniformLocation(programId, l4), lightColor.x, lightColor.y, lightColor.z,1.0f);
        glUniform1i(glGetUniformLocation(programId, "lightNum"), lights.size());
    }
}

void Shader::activate() {
    if(activeShader != programId) {
        glUseProgram(programId);
        activeShader = programId;
    }

    if(lightsChanged) loadLights();

    if(cameraMovedFlag) {
        cameraMovedFlag = false;
        glUniformMatrix4fv(glGetUniformLocation(programId, "camMatrix"), 1, GL_FALSE,glm::value_ptr(camera->getCameraMatrix()));
    }
}

void Shader::unloadFiles() {
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}

Shader::~Shader() {
    glDeleteProgram(programId);
}

GLuint Shader::getProgramID() {
    return programId;
}

void Shader::setCamera(Camera &camera) {
    this->camera = &camera;
}

void Shader::addLight(Light &light) {
    if(lights.size() >= 16) throw std::invalid_argument("More than 16 light sources cannot be linked to a single object.");
    if(std::find(lights.begin(), lights.end(), &light) != lights.end()) return;
    lights.push_back(&light);
    lightsChanged = true;
}

void Shader::removeLight(int index) {
    if(lights.empty()) return;
    if(index == -1) {
        lights.pop_back();
        lightsChanged = true;
        return;
    }
    if(lights.size() <= index) return;

    lights.erase(lights.begin() + index);
    lightsChanged = true;
}

void Shader::removeLight(Light &light) {
    auto result = std::find(lights.begin(), lights.end(), &light);
    if(result != lights.end()) {
        lights.erase(result);
        lightsChanged = true;
    }
}
