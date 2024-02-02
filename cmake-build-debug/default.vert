#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCol;
layout (location = 2) in vec2 aTex;

out vec4 color;
out vec2 texCoord;

uniform float scale;
uniform mat4 camMatrix;
uniform mat4 position;
uniform mat4 rotation;

void main() {
   gl_Position = camMatrix * position * rotation * vec4(aPos * scale, 1.0);
   color = aCol;
   texCoord = aTex;
}