#version 330 core

uniform vec4 uColor;

out vec4 fragmentColor;

void main() {
    float gray = (uColor.r + uColor.g + uColor.b) / 3.0;

    fragmentColor = vec4(gray, gray, gray, uColor.a);
}