#version 330 core
out vec4 FragColor;
in vec3 color;
in vec3 texCoord;
in vec3 normal;
in vec3 pos;

uniform sampler2D diffuseTextures[16];
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;
uniform sampler2D specularTextures[16];
uniform vec3 lightDir;
uniform int lightingType;
uniform bool useTexture;

vec4 selectDiffuseTexture(int i) {
    switch(i) {
        case 0:
            return texture(diffuseTextures[0], vec2(texCoord.x, texCoord.y));
        case 1:
            return texture(diffuseTextures[1], vec2(texCoord.x, texCoord.y));
        case 2:
            return texture(diffuseTextures[2], vec2(texCoord.x, texCoord.y));
        case 3:
            return texture(diffuseTextures[3], vec2(texCoord.x, texCoord.y));
        case 4:
            return texture(diffuseTextures[4], vec2(texCoord.x, texCoord.y));
        case 5:
            return texture(diffuseTextures[5], vec2(texCoord.x, texCoord.y));
        case 6:
            return texture(diffuseTextures[6], vec2(texCoord.x, texCoord.y));
        case 7:
            return texture(diffuseTextures[7], vec2(texCoord.x, texCoord.y));
        case 8:
            return texture(diffuseTextures[8], vec2(texCoord.x, texCoord.y));
        case 9:
            return texture(diffuseTextures[9], vec2(texCoord.x, texCoord.y));
        case 10:
            return texture(diffuseTextures[10], vec2(texCoord.x, texCoord.y));
        case 11:
            return texture(diffuseTextures[11], vec2(texCoord.x, texCoord.y));
        case 12:
            return texture(diffuseTextures[12], vec2(texCoord.x, texCoord.y));
        case 13:
            return texture(diffuseTextures[13], vec2(texCoord.x, texCoord.y));
        case 14:
            return texture(diffuseTextures[14], vec2(texCoord.x, texCoord.y));
        case 15:
            return texture(diffuseTextures[15], vec2(texCoord.x, texCoord.y));
    }
}

vec4 selectSpecularTexture(int i) {
    switch(i) {
        case 0:
            return texture(specularTextures[0], vec2(texCoord.x, texCoord.y));
        case 1:
            return texture(specularTextures[1], vec2(texCoord.x, texCoord.y));
        case 2:
            return texture(specularTextures[2], vec2(texCoord.x, texCoord.y));
        case 3:
            return texture(specularTextures[3], vec2(texCoord.x, texCoord.y));
        case 4:
            return texture(specularTextures[4], vec2(texCoord.x, texCoord.y));
        case 5:
            return texture(specularTextures[5], vec2(texCoord.x, texCoord.y));
        case 6:
            return texture(specularTextures[6], vec2(texCoord.x, texCoord.y));
        case 7:
            return texture(specularTextures[7], vec2(texCoord.x, texCoord.y));
        case 8:
            return texture(specularTextures[8], vec2(texCoord.x, texCoord.y));
        case 9:
            return texture(specularTextures[9], vec2(texCoord.x, texCoord.y));
        case 10:
            return texture(specularTextures[10], vec2(texCoord.x, texCoord.y));
        case 11:
            return texture(specularTextures[11], vec2(texCoord.x, texCoord.y));
        case 12:
            return texture(specularTextures[12], vec2(texCoord.x, texCoord.y));
        case 13:
            return texture(specularTextures[13], vec2(texCoord.x, texCoord.y));
        case 14:
            return texture(specularTextures[14], vec2(texCoord.x, texCoord.y));
        case 15:
            return texture(specularTextures[15], vec2(texCoord.x, texCoord.y));
    }
}

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

    if(useTexture)
        return (selectDiffuseTexture(int(texCoord.z)) * min(diffuse + ambient, 1) * dropoff + selectSpecularTexture(int(texCoord.z)).r * spec * dropoff) * lightColor;

    return (vec4(color, 1) * (diffuse + ambient + spec) * dropoff) * lightColor;
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

    if(useTexture)
        return (selectDiffuseTexture(int(texCoord.z)) * min(diffuse + ambient, 1) + selectSpecularTexture(int(texCoord.z)).r * spec) * lightColor;

    return vec4(color, 1) * (diffuse + ambient + spec) * lightColor;
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

    if(useTexture)
        return (selectDiffuseTexture(int(texCoord.z)) * min(diffuse + ambient, 1) * dropoff * inten + selectSpecularTexture(int(texCoord.z)).r * spec * dropoff * inten) * lightColor;

    return (vec4(color, 1) * (diffuse + ambient + spec) * dropoff * inten) * lightColor;
}

void main() {
    switch(lightingType) {
            case 0:
            FragColor = pointLight();
            break;

            case 1:
            //FragColor = directionalLight();
            break;

            case 2:
            //FragColor = spotLight();
            break;
    }
}