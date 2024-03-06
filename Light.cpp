#include "Light.h"
#include "TextureHandler.h"

#include <utility>

Light::Light(glm::vec3 color, glm::vec3 direction, LightingType type, unsigned int shadowWidth, unsigned int shadowHeight)
: color(color), type(type), shadowWidth(shadowWidth), shadowHeight(shadowHeight),
View(shadowWidth, shadowHeight, 90.0f, .1f, 100.0f) {
    setOrientation(direction);
    //Set default parameters based on type
    if(type == DirectionalLight) setOrthographicBorder(-5.0f, 5.0f, -5.0f, 5.0f);
    else if(type == SpotLight) setFov(10.0f);

    glGenFramebuffers(1, &shadowBuffer);

    //Initialize texture for the shadow map
    shadowMap = new Texture(TextureInfo());
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1, 1, 1, 1};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //Bind texture to the framebuffer and disable color rendering
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    shadowMap->setId(textureId);
}

glm::vec3 Light::getColor() const {
    return color;
}

void Light::setColor(glm::vec3 color) {
    Light::color = color;
}

LightingType Light::getType() const {
    return type;
}

void Light::setType(LightingType type) {
    Light::type = type;
    //Set default parameters based on type
    switch(type) {
        case PointLight:
            setFov(90.0f);
            break;
        case DirectionalLight:
            setOrthographicBorder(-5.0f, 5.0f, -5.0f, 5.0f);
            break;
        case SpotLight:
            setFov(10.0f);
            outerCone = glm::cos(glm::radians(10.0f));
            innerCone = glm::cos(glm::radians(5.0f));
            break;
        default:
            throw std::invalid_argument("Invalid lighting type given");
    }
}

void Light::setType(LightingType type, float nearPlane, float farPlane, float left_fov, float right, float bottom, float top) {
    this->type = type;
    if(type < PointLight || type > SpotLight) throw std::invalid_argument("Invalid lighting type given");

    if(type == DirectionalLight) setOrthographicBorder(left_fov, right, top, bottom);
    else {
        setFov(left_fov);
        if(type == SpotLight) {
            outerCone = glm::cos(glm::radians(left_fov));
            innerCone = glm::cos(glm::radians(std::min(left_fov - 5, 0.0f)));
        }
    }
    setNearPlane(nearPlane);
    setFarPlane(farPlane);
}

void Light::drawShadowMap() {
    glViewport(0, 0, shadowWidth, shadowHeight);
    activate(); // Update view matrix and set view of the light as the active one
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUniformMatrix4fv(glGetUniformLocation(Shader::getActiveShader(), "lightMatrix"), 1, false,glm::value_ptr(getCameraMatrix()));
}

Texture &Light::getShadowMap() const {
    return *shadowMap;
}

void Light::loadLight(int index) {
    glm::vec3 lightSource = getPosition();
    glm::vec3 lightDir = getOrientation();
    glm::vec3 lightColor = color;

    char fieldName[20];
    sprintf(fieldName, "lightPos[%d]", index);
    glUniform3f(glGetUniformLocation(Shader::getActiveShader(), fieldName), lightSource.x, lightSource.y,lightSource.z);

    sprintf(fieldName, "lightingType[%d]", index);
    glUniform1i(glGetUniformLocation(Shader::getActiveShader(), fieldName),type);

    sprintf(fieldName, "lightDir[%d]", index);
    glUniform3f(glGetUniformLocation(Shader::getActiveShader(), fieldName), lightDir.x, lightDir.y, lightDir.z);

    sprintf(fieldName, "lightColor[%d]", index);
    glUniform4f(glGetUniformLocation(Shader::getActiveShader(), fieldName), lightColor.x, lightColor.y, lightColor.z,1.0f);

    sprintf(fieldName, "lightMatrix[%d]", index);
    glUniformMatrix4fv(glGetUniformLocation(Shader::getActiveShader(), fieldName), 1, false, glm::value_ptr(getCameraMatrix()));

    sprintf(fieldName, "shadowMap[%d]", index);
    glUniform1i(glGetUniformLocation(Shader::getActiveShader(), fieldName),shadowMap->getInfo().type);

    sprintf(fieldName, "lightDropoffA[%d]", index);
    glUniform1f(glGetUniformLocation(Shader::getActiveShader(), fieldName),a);

    sprintf(fieldName, "lightDropoffB[%d]", index);
    glUniform1f(glGetUniformLocation(Shader::getActiveShader(), fieldName),b);

    sprintf(fieldName, "spotlightInnerCone[%d]", index);
    glUniform1f(glGetUniformLocation(Shader::getActiveShader(), fieldName),innerCone);

    sprintf(fieldName, "spotlightOuterCone[%d]", index);
    glUniform1f(glGetUniformLocation(Shader::getActiveShader(), fieldName),outerCone);
}

unsigned int Light::getShadowWidth() const {
    return shadowWidth;
}

unsigned int Light::getShadowHeight() const {
    return shadowHeight;
}

void Light::setAttenuationParams(float a, float b) {
    this->a = a;
    this->b = b;
}

void Light::setSpotlightParams(float innerCone, float outerCone) {
    this->innerCone = glm::cos(glm::radians(innerCone));
    this->outerCone = glm::cos(glm::radians(outerCone));
}

void Light::setFov(float value) {
    View::setFov(value);
    innerCone = glm::cos(std::max(glm::radians(value - 5), 0.0f));
    outerCone = glm::cos(glm::radians(value));
}
