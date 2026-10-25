#version 330 core

uniform vec4 uColor;

out vec4 fragmentColor;

void main() {
    fragmentColor = uColor;
}