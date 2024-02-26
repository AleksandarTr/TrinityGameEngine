#include "Light.h"

#include <utility>

Light::Light(glm::vec3 color, glm::vec3 direction, LightingType type) : color(color), type(type), direction(direction) {
    glGenFramebuffers(1, &shadowBuffer);

    shadowMap = new Texture(TextureInfo());
    glGenTextures(1, &shadowMap->textureId);
    glBindTexture(GL_TEXTURE_2D, shadowMap->textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1, 1, 1, 1};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap->textureId, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const glm::vec3 &Light::getColor() const {
    return color;
}

void Light::setColor(const glm::vec3 color) {
    Light::color = color;
}

const glm::vec3 &Light::getDirection() const {
    return direction;
}

void Light::setDirection(const glm::vec3 direction) {
    Light::direction = direction;
}

LightingType Light::getType() const {
    return type;
}

void Light::setType(LightingType type) {
    Light::type = type;
}

void Light::drawShadowMap() {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);

    float FOV;
    if (type == LightingType::SpotLight) FOV = 10;
    else FOV = 90;

    glm::mat4 lightProjection;
    float nearPlane = 0.1f, farPlane = 100.0f;
    if (type == LightingType::DirectionalLight)
        lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, nearPlane, farPlane);
    else lightProjection = glm::perspective(glm::radians(FOV), 1.0f * shadowWidth / shadowHeight, nearPlane, farPlane);

    glm::mat4 lightView = glm::lookAt(getPosition(), getPosition() + direction, glm::vec3(0,1,0));
    lightMatrix = lightProjection * lightView;

    glUniformMatrix4fv(glGetUniformLocation(Shader::getActiveShader(), "lightMatrix"), 1, false,glm::value_ptr(lightMatrix));
}

Texture &Light::getShadowMap() {
    return *shadowMap;
}

const glm::mat4& Light::getLightMatrix() {
    return lightMatrix;
}
