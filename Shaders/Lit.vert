#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform mat4 uViewProjection;
uniform mat4 uModel;
uniform vec2 uUvScale;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vWorldPosition;

void main() {
	vec4 worldPosition = uModel * vec4(aPosition, 1.0);

	gl_Position = uViewProjection * worldPosition;
	vWorldPosition = worldPosition.xyz;
	vTexCoord = aTexCoord * uUvScale;
	vNormal = transpose(inverse(mat3(uModel))) * aNormal;
}