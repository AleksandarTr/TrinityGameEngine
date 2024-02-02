#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 normal;
in vec3 pos;

uniform sampler2D tex0;
uniform vec4 lightColor;
uniform vec3 lightPos;

void main() {
    vec3 norm = normalize(normal);
    vec3 light = normalize(lightPos - pos);
    float diffuse = max(dot(norm, light), 0.1f);

   FragColor = texture(tex0, texCoord) * lightColor * diffuse;
}