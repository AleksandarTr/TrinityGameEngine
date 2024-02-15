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
uniform float metallic;
uniform float roughness;

uniform vec4 lightColor[16];
uniform vec3 lightPos[16];
uniform vec3 lightDir[16];
uniform int lightingType[16];
uniform int lightNum;

uniform vec3 camPos;
vec3 normalCalc;
vec4 diffuseVector;
vec4 specularVector;

const float PI = 3.14159265359f;
vec4 lighting[2] = {vec4(0, 0, 0, 0), vec4(0, 0, 0, 0)};
int lightInd = 0;
float maxSpec = 1;

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

vec4 calcSpec(vec3 lightVec, vec3 norm) {
    vec3 viewDir = normalize(camPos - pos);
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, diffuseVector.rgb, specularVector.b);
    vec3 halfVec = normalize(lightVec + viewDir);

    float NDF = DistributionGGX(norm, halfVec, specularVector.r);
    float G = GeometrySmith(norm, viewDir, lightDir[lightInd], specularVector.r);
    vec3 F = fresnelSchlick(max(dot(halfVec, viewDir), 0), F0);

    vec3 kD = vec3(1.0f) - F;
    kD *= 1.0 - specularVector.b;
    vec3 specular = (NDF * G / (4.0 * max(dot(norm, viewDir), 0.0) * max(dot(norm, lightDir[lightInd]), 0.0) + 0.0001)) * F;
    return vec4(specular, 1.0f);
}

void pointLight() {
    vec3 lightVec = normalize(lightPos[lightInd] - pos);
    vec3 norm = normalize(normalCalc);
    float diffuse = calcDiffuse(lightVec, norm);
    float ambient = calcAmbient();
    float dropoff = calcDropoff(lightVec);
    vec4 spec = calcSpec(lightVec, norm);

    lighting[0] = max(min(diffuse + ambient, 1) * dropoff, 0) * lightColor[lightInd];
    lighting[1] = diffuse * dropoff * spec * lightColor[lightInd];
}

void directionalLight() {
    vec3 lightVec = normalize(lightPos[lightInd]);
    vec3 norm = normalize(normalCalc);
    float diffuse = calcDiffuse(lightVec, norm);
    float ambient = calcAmbient();
    vec4 spec = calcSpec(lightVec, norm);

    lighting[0] = min(diffuse + ambient, 1) * lightColor[lightInd];
    lighting[1] = spec * lightColor[lightInd];
}

void spotLight() {
    float outerCone = 0.99f;
    float innerCone = 0.999f;
    vec3 lightVec = normalize(lightPos[lightInd] - pos);
    vec3 norm = normalize(normalCalc);
    float diffuse = calcDiffuse(lightVec, norm);
    float ambient = calcAmbient();
    float dropoff = calcDropoff(lightVec);
    vec4 spec = calcSpec(lightVec, norm);

    float angle = dot(normalize(lightDir[lightInd]), - lightVec);
    float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0, 1.0f);

    lighting[0] = min(diffuse + ambient, 1) * dropoff * inten * lightColor[lightInd];
    lighting[1] = dropoff * inten * spec * lightColor[lightInd];
}

void main() {
    normalCalc = applyNormalTexture ? TBN * (texture(normalTexture, vec2(texCoord)).rgb * 2.0 - 1.0) : normal;
    diffuseVector = useTexture ? texture(diffuseTexture, vec2(texCoord)).rgba : vec4(color, 1.0f);

    specularVector = applySpecularTexture ? texture(specularTexture, vec2(texCoord)).rgba : vec4(roughness, metallic, 1.0f, 1.0f);

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

    FragColor = diffuseVector * resLight1 + resLight2;
}