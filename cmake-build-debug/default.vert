#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNorm;

out vec3 color;
out vec2 texCoord;
out vec3 normal;
out vec3 pos;

uniform float scale;
uniform mat4 camMatrix;
uniform mat4 position;
uniform mat4 rotation;

void main() {
   gl_Position = camMatrix * position * rotation * vec4(aPos * scale, 1.0);
   texCoord = aTex;
   normal = vec3(rotation * vec4(aNorm, 1.0f));
   pos = vec3(position * rotation * vec4(aPos * scale, 1.0f));
}