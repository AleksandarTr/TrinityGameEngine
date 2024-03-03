#include "Light.h"
#include "TextureHandler.h"

#include <utility>

Light::Light(glm::vec3 color, glm::vec3 direction, LightingType type, unsigned int shadowWidth, unsigned int shadowHeight)
: color(color), type(type), shadowWidth(shadowWidth), shadowHeight(shadowHeight),
View(shadowWidth, shadowHeight, 90.0f, .1f, 100.0f) {
    setOrientation(direction);
    if(type == DirectionalLight) setOrthographicBorder(-5.0f, 5.0f, -5.0f, 5.0f);
    else if(type == SpotLight) setFov(10.0f);

    glGenFramebuffers(1, &shadowBuffer);

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
    switch(type) {
        case PointLight:
            setFov(90.0f);
            break;
        case DirectionalLight:
            setOrthographicBorder(-5.0f, 5.0f, -5.0f, 5.0f);
            break;
        case SpotLight:
            setFov(10.0f);
            break;
        default:
            throw std::invalid_argument("Invalid lighting type given");
    }
}

void Light::setType(LightingType type, float nearPlane, float farPlane, float left_fov, float right, float bottom, float top) {
    this->type = type;
    if(type < PointLight || type > SpotLight) throw std::invalid_argument("Invalid lighting type given");

    if(type == DirectionalLight) setOrthographicBorder(left_fov, right, top, bottom);
    else setFov(left_fov);
    setNearPlane(nearPlane);
    setFarPlane(farPlane);
}

void Light::drawShadowMap() {
    glViewport(0, 0, shadowWidth, shadowHeight);
    activate();
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
}

unsigned int Light::getShadowWidth() const {
    return shadowWidth;
}

unsigned int Light::getShadowHeight() const {
    return shadowHeight;
}
