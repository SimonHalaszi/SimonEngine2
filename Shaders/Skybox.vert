#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uViewProjection;
uniform mat4 uModel;

out vec3 vCubeDirection;

void main() {
	vec4 clipPosition = uViewProjection * uModel * vec4(aPosition, 1.0);
	gl_Position = clipPosition.xyww;
	vCubeDirection = aPosition;
}