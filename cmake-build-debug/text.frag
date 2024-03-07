in vec3 color;
in vec3 texCoord;
out vec4 FragColor;

uniform sampler2D diffuseTexture;

void main() {
    FragColor = texture(diffuseTexture, vec2(texCoord)) * vec4(color, 1);
    if(FragColor.a < 0.5) discard;
}