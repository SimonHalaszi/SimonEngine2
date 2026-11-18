#version 330 core

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
	float range;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
	float range;
	float innerConeCos;
	float outerConeCos;
};

const int MAX_DIRECTIONAL_LIGHTS = 2;
const int MAX_POINT_LIGHTS = 8;
const int MAX_SPOT_LIGHTS = 8;

uniform DirectionalLight uDirectionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform int uDirectionalLightCount;

uniform PointLight uPointLights[MAX_POINT_LIGHTS];
uniform int uPointLightCount;

uniform SpotLight uSpotLights[MAX_SPOT_LIGHTS];
uniform int uSpotLightCount;

uniform vec4 uColor;
uniform sampler2D uTexture;
uniform bool uHasTexture;

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vWorldPosition;

out vec4 fragmentColor;

float localLightFalloff(vec3 toLight, float range, out vec3 towardLight) {
	float distanceToLight = length(toLight);

	if (distanceToLight <= 0.0001 || distanceToLight >= range) {
		towardLight = vec3(0.0);
		return 0.0;
	}

	towardLight = toLight / distanceToLight;
	return 1.0 - smoothstep(0.0, range, distanceToLight);
}

void main() {
	vec4 baseColor = uColor;

	if (uHasTexture) {
		baseColor *= texture(uTexture, vTexCoord);
	}

	vec3 normal = normalize(vNormal);
	vec3 illumination = vec3(0.05);

	for (int i = 0; i < uDirectionalLightCount; ++i) {
		DirectionalLight light = uDirectionalLights[i];
		vec3 towardLight = -normalize(light.direction);
		float diffuse = max(dot(normal, towardLight), 0.0);

		illumination += light.color * light.intensity * diffuse;
	}

	for (int i = 0; i < uPointLightCount; ++i) {
		PointLight light = uPointLights[i];
		vec3 towardLight;
		float falloff = localLightFalloff(light.position - vWorldPosition, light.range, towardLight);
		float diffuse = max(dot(normal, towardLight), 0.0);

		illumination += light.color * light.intensity * diffuse * falloff;
	}

	for (int i = 0; i < uSpotLightCount; ++i) {
		SpotLight light = uSpotLights[i];
		vec3 towardLight;
		float falloff = localLightFalloff(light.position - vWorldPosition, light.range, towardLight);
		float diffuse = max(dot(normal, towardLight), 0.0);
		float coneCos = dot(light.direction, -towardLight);
		float cone = smoothstep(light.outerConeCos, light.innerConeCos, coneCos);

		illumination += light.color * light.intensity * diffuse * falloff * cone;
	}

	fragmentColor = vec4(baseColor.rgb * illumination, baseColor.a);
}