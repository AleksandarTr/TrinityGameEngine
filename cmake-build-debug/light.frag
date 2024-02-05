#version 330 core
out vec4 FragColor;
in vec3 color;

uniform vec3 lightColor;
uniform bool useLightColor;

void main() {
   if(useLightColor) FragColor = vec4(lightColor, 1);
   else FragColor = vec4(color, 1);
}