#include "Light.h"

#include <utility>

Light::Light(glm::vec3 color, glm::vec3 direction, LightingType type) : color(color), type(type), direction(direction) {
    glGenFramebuffers(1, &shadowBuffer);

    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
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
    glViewport(0, 0, shadowWidth, shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
