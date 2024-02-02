#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 camMatrix;
uniform mat4 position;
uniform float scale;

void main() {
   gl_Position = camMatrix * position * vec4(aPos * scale, 1.0);
}