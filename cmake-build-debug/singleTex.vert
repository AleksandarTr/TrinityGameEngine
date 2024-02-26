#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aTex;
layout (location = 3) in vec3 aNorm;
layout (location = 4) in vec3 aTan;
layout (location = 5) in vec3 aBitan;

#define maxLightCount 16

out vec3 color;
out vec3 texCoord;
out vec3 normal;
out vec3 pos;
out mat3 TBN;
out vec4 lightFragPos[maxLightCount];

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 rotation;
uniform int lightNum;
uniform mat4 lightMatrix[maxLightCount];

void main() {
    pos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = camMatrix * vec4(pos, 1.0f);
    texCoord = aTex;
    color = aCol;
    normal = vec3(rotation * vec4(aNorm, 1));

    for(int i = 0; i < lightNum; i++) lightFragPos[i] = lightMatrix[i] * vec4(pos, 1.0f);

    vec3 T = normalize(vec3(model * vec4(aTan, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBitan, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNorm, 0.0)));
    TBN = mat3(T, B, N);
}