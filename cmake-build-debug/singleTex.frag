#version 330 core
out vec4 FragColor;
in vec3 color;
in vec3 texCoord;
in vec3 normal;
in vec3 pos;
in mat3 TBN;

uniform sampler2D diffuseTexture;
uniform bool useTexture;
uniform sampler2D specularTexture;
uniform bool applySpecularTexture;
uniform sampler2D normalTexture;
uniform bool applyNormalTexture;
uniform sampler2D occlusionTexture;
uniform bool applyOcclusionTexture;
uniform sampler2D emissionTexture;
uniform bool applyEmissionTexture;

uniform bool applyColor;
uniform float metallic;
uniform float roughness;
uniform float occlusionStrength;
uniform float normalScale;
uniform float alphaCutoff;
uniform vec3 emissionValues;

#define maxLightCount 16
uniform vec4 lightColor[maxLightCount];
uniform vec3 lightPos[maxLightCount];
uniform vec3 lightDir[maxLightCount];
uniform int lightingType[maxLightCount];
uniform float lightDropoffA[maxLightCount];
uniform float lightDropoffB[maxLightCount];
uniform float spotlightInnerCone[maxLightCount];
uniform float spotlightOuterCone[maxLightCount];
uniform int lightNum;
in vec4 lightFragPos[maxLightCount];
uniform sampler2D shadowMap[maxLightCount];

uniform vec3 camPos;
vec3 normalCalc;
vec3 diffuseVector;
vec3 specularVector;

const float PI = 3.14159265359f;
vec3 lighting = vec3(0, 0, 0);
int lightInd = 0;

float calcDropoff(vec3 lightVec) {
    float a = lightDropoffA[lightInd];
    float b = lightDropoffB[lightInd];
    float dist = length(lightVec);
    return 1.0f / (a * dist * dist + b * dist + 1.0f);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void pointLight() {
    vec3 lightVec = normalize(lightPos[lightInd] - pos);
    vec3 normalVec = normalize(normalCalc);
    float dropoff = calcDropoff(lightVec);

    vec3 viewDir = normalize(camPos - pos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, diffuseVector, specularVector.b);
    vec3 halfVec = normalize(lightVec + viewDir);

    float NDF = DistributionGGX(normalVec, halfVec, specularVector.g);
    float G = GeometrySmith(normalVec, viewDir, lightVec, specularVector.g);
    vec3 F = fresnelSchlick(max(dot(halfVec, viewDir), 0), F0);

    vec3 kD = vec3(1.0f) - F;
    kD *= 1.0 - specularVector.b;
    vec3 specular = (NDF * G / (4.0 * max(dot(normalVec, viewDir), 0.0) * max(dot(normalVec, lightVec), 0.0) + 0.0001)) * F;

    float NdotL = max(dot(normalVec, lightVec), 0);
    lighting = (kD * diffuseVector / PI + specular) * dropoff * NdotL * lightColor[lightInd].rgb;
}

void directionalLight() {
    vec3 lightVec = normalize(lightPos[lightInd]);
    vec3 normalVec = normalize(normalCalc);

    vec3 viewDir = normalize(camPos - pos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, diffuseVector, specularVector.b);
    vec3 halfVec = normalize(lightVec + viewDir);

    float NDF = DistributionGGX(normalVec, halfVec, specularVector.g);
    float G = GeometrySmith(normalVec, viewDir, lightVec, specularVector.g);
    vec3 F = fresnelSchlick(max(dot(halfVec, viewDir), 0), F0);

    vec3 kD = vec3(1.0f) - F;
    kD *= 1.0 - specularVector.b;
    vec3 specular = (NDF * G / (4.0 * max(dot(normalVec, viewDir), 0.0) * max(dot(normalVec, lightVec), 0.0) + 0.0001)) * F;

    float NdotL = max(dot(normalVec, lightVec), 0);
    lighting = (kD * diffuseVector / PI + specular) * NdotL * lightColor[lightInd].rgb;
}

void spotLight() {
    float outerCone = spotlightOuterCone[lightInd];
    float innerCone = spotlightInnerCone[lightInd];
    vec3 lightVec = normalize(lightPos[lightInd] - pos);

    float angle = dot(normalize(lightDir[lightInd]), - lightVec);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0, 1.0f);

    pointLight();
    lighting *= inten;
}

#define calcShadow(i) texelSize = 1.0f / textureSize(shadowMap[i], 0); \
    for(int x = -1; x <= 1; x++) { \
      for(int y = -1; y <= 1; y++) { \
          float pcfDepth = texture(shadowMap[i], projCoords.xy + vec2(x,y) * texelSize).r; \
          shadow += projCoords.z > pcfDepth ? 1.0 : 0; \
      } \
    }

float calculateShadow(vec4 position) {
    vec3 projCoords = position.xyz / position.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0f) return 0;

    float shadow = 0;
    vec2 texelSize;

    switch(lightInd) {
        case 0:
            calcShadow(0);
            break;
        case 1:
            calcShadow(1);
            break;
        case 2:
            calcShadow(2);
            break;
        case 3:
            calcShadow(3);
            break;
        case 4:
            calcShadow(4);
            break;
        case 5:
            calcShadow(5);
            break;
        case 6:
            calcShadow(6);
            break;
        case 7:
            calcShadow(7);
            break;
        case 8:
            calcShadow(8);
            break;
        case 9:
            calcShadow(9);
            break;
        case 10:
            calcShadow(10);
            break;
        case 11:
            calcShadow(11);
            break;
        case 12:
            calcShadow(12);
            break;
        case 13:
            calcShadow(13);
            break;
        case 14:
            calcShadow(14);
            break;
        case 15:
            calcShadow(15);
            break;
    }

    return shadow / 9;
}

void main() {
    vec4 diffuseTextureVector = texture(diffuseTexture, vec2(texCoord));
    normalCalc = applyNormalTexture ? TBN * ((texture(normalTexture, vec2(texCoord)).rgb * 2.0 - 1.0) * vec3(normalScale, normalScale, 1.0f)) : normal;
    diffuseVector = useTexture ? pow(diffuseTextureVector.rgb, vec3(2.2)) : vec3(color);
    if(useTexture && diffuseTextureVector.a < alphaCutoff) discard;

    specularVector = applySpecularTexture ? texture(specularTexture, vec2(texCoord)).rgb : vec3(1.0f, metallic, roughness);
    specularVector.r = applyOcclusionTexture ? 1.0f + occlusionStrength * (texture(occlusionTexture, vec2(texCoord)).r - 1.0f) : 1.0f;

    vec3 resLight = vec3(0, 0, 0);
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

        resLight = resLight + lighting * (1.0f - calculateShadow(lightFragPos[i]));
    }

    resLight.x = min(resLight.x, 1);
    resLight.y = min(resLight.y, 1);
    resLight.z = min(resLight.z, 1);

    resLight *= specularVector.r;
    resLight = resLight / (resLight + vec3(1.0));
    resLight = pow(resLight, vec3(1.0/2.2));

    vec3 emissionVector = applyEmissionTexture ? texture(emissionTexture, vec2(texCoord)).rgb : emissionValues;
    resLight += emissionVector;

    FragColor = vec4(resLight, diffuseTextureVector.a);
    if(applyColor && useTexture) FragColor *= vec4(color, 1);
}