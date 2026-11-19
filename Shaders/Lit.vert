#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec4 aTangent;

uniform mat4 uViewProjection;
uniform mat4 uModel;
uniform vec2 uUvScale;

out vec2 vTexCoord;
out vec3 vNormal;
out vec4 vTangent;
out vec3 vWorldPosition;

void main() {
	vec4 worldPosition = uModel * vec4(aPosition, 1.0);
	mat3 normalMatrix = transpose(inverse(mat3(uModel)));
	vec3 normal = normalize(normalMatrix * aNormal);
	vec3 tangent = mat3(uModel) * aTangent.xyz;
	tangent = normalize(tangent - normal * dot(normal, tangent));

	gl_Position = uViewProjection * worldPosition;
	vWorldPosition = worldPosition.xyz;
	vTexCoord = aTexCoord * uUvScale;
	vNormal = normal;
	vTangent = vec4(tangent, aTangent.w * sign(determinant(mat3(uModel))));
}