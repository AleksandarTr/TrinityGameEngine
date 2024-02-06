#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec3 aTex;
layout (location = 3) in vec3 aNorm;

out vec3 color;
out vec3 texCoord;
out vec3 normal;
out vec3 pos;

uniform mat4 model;
uniform mat4 camMatrix;
uniform mat4 rotation;

void main() {
   pos = vec3(model * vec4(aPos, 1.0f));
   gl_Position = camMatrix * vec4(pos, 1.0f);
   texCoord = aTex;
   color = aCol;
   normal = vec3(rotation * vec4(aNorm, 1.0f));
}