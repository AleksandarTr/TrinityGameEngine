in vec3 color;
in vec3 texCoord;
out vec4 FragColor;

uniform sampler2D font;

void main() {
    FragColor = texture(font, vec2(texCoord)) * vec4(color, 1);
    if(FragColor.a < 0.5) discard;
}