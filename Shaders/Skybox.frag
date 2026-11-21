#version 330 core

uniform samplerCube uCubeMap;

in vec3 vCubeDirection;

out vec4 fragmentColor;

void main() {
	fragmentColor = texture(uCubeMap, normalize(vCubeDirection));
}