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

uniform bool applyColor;
uniform float metallic;
uniform float roughness;
float ambientOcclusion = 0.1f;

uniform vec4 lightColor[16];
uniform vec3 lightPos[16];
uniform vec3 lightDir[16];
uniform int lightingType[16];
uniform int lightNum;

uniform vec3 camPos;
vec3 normalCalc;
vec3 diffuseVector;
vec3 specularVector;

const float PI = 3.14159265359f;
vec3 lighting = vec3(0, 0, 0);
int lightInd = 0;
float maxSpec = 1;

float calcDropoff(vec3 lightVec) {
    float a = 0.01f;
    float b = 0.005f;
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
    float outerCone = 0.99f;
    float innerCone = 0.999f;
    vec3 lightVec = normalize(lightPos[lightInd] - pos);

    float angle = dot(normalize(lightDir[lightInd]), - lightVec);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0, 1.0f);

    pointLight();
    lighting *= inten;
}

void main() {
    normalCalc = applyNormalTexture ? TBN * (texture(normalTexture, vec2(texCoord)).rgb * 2.0 - 1.0) : normal;
    diffuseVector = useTexture ? pow(texture(diffuseTexture, vec2(texCoord)).rgb, vec3(2.2)) : vec3(color);

    specularVector = applySpecularTexture ? texture(specularTexture, vec2(texCoord)).rgb : vec3(1.0f, metallic, roughness);
    specularVector.r = applyOcclusionTexture ? texture(occlusionTexture, vec2(texCoord)).r : ambientOcclusion;

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

        resLight = resLight + lighting;
    }

    resLight.x = min(resLight.x, 1);
    resLight.y = min(resLight.y, 1);
    resLight.z = min(resLight.z, 1);

    vec3 ambient = vec3(specularVector.r) * diffuseVector * vec3(0.03);
    resLight = resLight + ambient;
    resLight = resLight / (resLight + vec3(1.0));
    resLight = pow(resLight, vec3(1.0/2.2));

    FragColor = vec4(resLight, texture(diffuseTexture, vec2(texCoord)).a);
    if(applyColor && useTexture) FragColor *= vec4(color, 1);
}