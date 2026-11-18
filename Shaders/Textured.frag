#version 330 core

uniform sampler2D uTexture;
uniform vec4 uColor;

in vec2 vTexCoord;
out vec4 fragmentColor;

void main() {
    fragmentColor = texture(uTexture, vTexCoord) * uColor;
}