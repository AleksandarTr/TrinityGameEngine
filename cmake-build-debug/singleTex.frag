#version 330 core
out vec4 FragColor;
in vec3 color;
in vec3 texCoord;
in vec3 normal;
in vec3 pos;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform vec4 lightColor[16];
uniform vec3 lightPos[16];
uniform vec3 lightDir[16];
uniform int lightingType[16];
uniform int lightNum;

uniform bool useTexture;
uniform vec3 camPos;

vec4 lighting[2] = {vec4(0, 0, 0, 0), vec4(0, 0, 0, 0)};
int lightInd = 0;
float maxSpec = 0.2;

float calcDiffuse(vec3 lightVec, vec3 norm) {
    return max(dot(norm, lightVec), 0);
}

float calcAmbient() {
    return 0.1f;
}

float calcDropoff(vec3 lightVec) {
    float a = 0.01f;
    float b = 0.005f;
    float dist = length(lightVec);
    return 1.0f / (a * dist * dist + b * dist + 1.0f);
}

float calcSpec(vec3 lightVec, vec3 norm) {
    float specMod = maxSpec;
    vec3 viewDir = normalize(camPos - pos);
    vec3 reflectionDir = reflect(-lightVec, norm);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0.0f), 8);
    return specAmount * specMod;
}

void pointLight() {
    vec3 lightVec = normalize(lightPos[lightInd] - pos);
    vec3 norm = normalize(normal);
    float diffuse = calcDiffuse(lightVec, norm);
    float ambient = calcAmbient();
    float dropoff = calcDropoff(lightVec);
    float spec = calcSpec(lightVec, norm);

    if(useTexture) {
        lighting[0] = max(min(diffuse + ambient, 1) * dropoff, 0) * lightColor[lightInd];
        lighting[1] = max(spec * dropoff * lightColor[lightInd], 0);
    }
    else lighting[0] = (diffuse + ambient + spec) * dropoff * lightColor[lightInd];
}

void directionalLight() {
    vec3 lightVec = normalize(lightPos[lightInd]);
    vec3 norm = normalize(normal);
    float diffuse = calcDiffuse(lightVec, norm);
    float ambient = calcAmbient();
    float spec = calcSpec(lightVec, norm);

    if(useTexture) {
        lighting[0] = min(diffuse + ambient, 1) * lightColor[lightInd];
        lighting[1] = spec * lightColor[lightInd];
    }
    else lighting[0] = (diffuse + ambient + spec) * lightColor[lightInd];
}

void spotLight() {
    float outerCone = 0.99f;
    float innerCone = 0.999f;
    vec3 lightVec = normalize(lightPos[lightInd] - pos);
    vec3 norm = normalize(normal);
    float diffuse = calcDiffuse(lightVec, norm);
    float ambient = calcAmbient();
    float dropoff = calcDropoff(lightVec);
    float spec = calcSpec(lightVec, norm);

    float angle = dot(normalize(lightDir[lightInd]), - lightVec);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0, 1.0f);

    if(useTexture) {
        lighting[0] = min(diffuse + ambient, 1) * dropoff * inten * lightColor[lightInd];
        lighting[1] = spec * dropoff * inten * lightColor[lightInd];
    }
    else lighting[0] = (diffuse + ambient + spec) * dropoff * inten * lightColor[lightInd];
}

void main() {
    vec4 resLight1 = vec4(0, 0, 0, 0);
    vec4 resLight2 = vec4(0, 0, 0, 0);
    for(int i = 0; i < lightNum; i++) {
        lightInd = i;
        switch (lightingType[i]) {
            case 0:
                pointLight();
                break;

            case 1:
                directionalLight();
                break;

            case 2:
                spotLight();
                break;
        }

        resLight1 = resLight1 + lighting[0];
        resLight2 = resLight2 + lighting[1];
    }

    resLight1.x = min(resLight1.x, 1);
    resLight1.y = min(resLight1.y, 1);
    resLight1.z = min(resLight1.z, 1);
    resLight1.w = min(resLight1.w, 1);

    resLight2.x = min(resLight2.x, maxSpec);
    resLight2.y = min(resLight2.y, maxSpec);
    resLight2.z = min(resLight2.z, maxSpec);
    resLight2.w = min(resLight2.w, maxSpec);

    if(useTexture) FragColor = texture(diffuseTexture, vec2(texCoord)) * resLight1 + texture(specularTexture, vec2(texCoord)).r * resLight2;
    else FragColor = vec4(color, 1) * resLight1;
}