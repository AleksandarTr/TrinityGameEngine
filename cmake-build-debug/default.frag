#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 normal;
in vec3 pos;

uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform sampler2D tex1;
uniform vec3 lightDir;
uniform int lightingType;

vec4 pointLight() {
    vec3 lightVec = lightPos - pos;

    vec3 norm = normalize(normal);
    vec3 light = normalize(lightVec);
    float diffuse = max(dot(norm, light), 0);

    float ambient = 0.1f;

    float a = 0.01f;
    float b = 0.005f;
    float dist = length(lightVec);
    float dropoff = 1.0f / (a * dist * dist + b * dist + 1.0f);

    float specMod = 0.5f;
    vec3 viewDir = normalize(camPos - pos);
    vec3 reflectionDir = reflect(-light, norm);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 8);
    float spec = specAmount * specAmount;

    return (texture(tex0, texCoord) * min(diffuse + ambient, 1) * dropoff + texture(tex1, texCoord).r * spec * dropoff) * lightColor;
}

vec4 directionalLight() {
    vec3 lightVec = lightPos;

    vec3 norm = normalize(normal);
    vec3 light = normalize(lightVec);
    float diffuse = max(dot(norm, light), 0);

    float ambient = 0.1f;

    float specMod = 0.5f;
    vec3 viewDir = normalize(camPos - pos);
    vec3 reflectionDir = reflect(-light, norm);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 8);
    float spec = specAmount * specAmount;

    return (texture(tex0, texCoord) * min(diffuse + ambient, 1) + texture(tex1, texCoord).r * spec) * lightColor;
}

vec4 spotLight() {
    float outerCone = 0.99f;
    float innerCone = 0.999f;
    vec3 lightVec = lightPos - pos;

    vec3 norm = normalize(normal);
    vec3 light = normalize(lightVec);
    float diffuse = max(dot(norm, light), 0);

    float ambient = 0.1f;

    float a = 0.001f;
    float b = 0.005f;
    float dist = length(lightVec);
    float dropoff = 1.0f / (a * dist * dist + b * dist + 1.0f);

    float specMod = 0.5f;
    vec3 viewDir = normalize(camPos - pos);
    vec3 reflectionDir = reflect(-light, norm);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 8);
    float spec = specAmount * specAmount;

    float angle = dot(normalize(lightDir), - light);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0, 1.0f);

    return (texture(tex0, texCoord) * min(diffuse + ambient, 1) * dropoff * inten + texture(tex1, texCoord).r * spec * dropoff * inten) * lightColor;
}

void main() {
    switch(lightingType) {
            case 0:
            FragColor = pointLight();
            break;

            case 1:
            FragColor = directionalLight();
            break;

            case 2:
            FragColor = spotLight();
            break;
    }
}