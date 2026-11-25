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
uniform sampler2D uNormalTexture;
uniform bool uHasNormalTexture;
uniform sampler2D uRoughnessTexture;
uniform bool uHasRoughnessTexture;
uniform sampler2D uMetallicTexture;
uniform bool uHasMetallicTexture;
uniform samplerCube uCubeMap;
uniform bool uHasCubeMap;
uniform float uRoughness;
uniform float uMetallic;
uniform vec3 uViewPosition;
uniform vec3 uAmbientColor;
uniform float uAmbientIntensity;

in vec2 vTexCoord;
in vec3 vNormal;
in vec4 vTangent;
in vec3 vWorldPosition;

out vec4 fragmentColor;

const float PI = 3.14159265359;

float localLightFalloff(vec3 toLight, float range, out vec3 towardLight) {
	float distanceToLight = length(toLight);

	if (distanceToLight <= 0.0001 || distanceToLight >= range) {
		towardLight = vec3(0.0);
		return 0.0;
	}

	towardLight = toLight / distanceToLight;
	return 1.0 - smoothstep(0.0, range, distanceToLight);
}

vec3 fresnelSchlick(float cosine, vec3 f0) {
	return f0 + (1.0 - f0) * pow(1.0 - cosine, 5.0);
}

float distributionGGX(vec3 normal, vec3 halfway, float roughness) {
	float alpha = roughness * roughness;
	float alphaSquared = alpha * alpha;
	float normalHalfway = clamp(dot(normal, halfway), 0.0, 1.0);
	vec3 normalCrossHalfway = cross(normal, halfway);
	float sinSquared = dot(normalCrossHalfway, normalCrossHalfway);
	float denominator = sinSquared + alphaSquared * normalHalfway * normalHalfway;

	return alphaSquared / (PI * denominator * denominator);
}

float geometrySchlickGGX(float normalDirection, float roughness) {
	float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
	return normalDirection / (normalDirection * (1.0 - k) + k);
}

vec3 evaluateLight(vec3 normal, vec3 viewDirection, vec3 lightDirection, vec3 radiance, vec3 albedo, float metallic, float roughness) {
	float normalLight = max(dot(normal, lightDirection), 0.0);
	float normalView = max(dot(normal, viewDirection), 0.0);

	if (normalLight <= 0.0 || normalView <= 0.0) {
		return vec3(0.0);
	}

	vec3 halfway = normalize(viewDirection + lightDirection);
	vec3 f0 = mix(vec3(0.04), albedo, metallic);
	vec3 fresnel = fresnelSchlick(max(dot(halfway, viewDirection), 0.0), f0);
	float distribution = distributionGGX(normal, halfway, roughness);
	float geometry = geometrySchlickGGX(normalView, roughness) * geometrySchlickGGX(normalLight, roughness);
	vec3 specular = distribution * geometry * fresnel / max(4.0 * normalView * normalLight, 0.0001);
	vec3 diffuseWeight = (1.0 - fresnel) * (1.0 - metallic);
	vec3 diffuse = diffuseWeight * albedo / PI;

	return (diffuse + specular) * radiance * normalLight;
}

void main() {
	vec4 baseColor = uColor;

	if (uHasTexture) {
		baseColor *= texture(uTexture, vTexCoord);
	}

	vec3 normal = normalize(vNormal);
	if (uHasNormalTexture) {
		vec3 tangent = normalize(vTangent.xyz);
		vec3 bitangent = normalize(cross(normal, tangent)) * vTangent.w;
		vec3 tangentNormal = texture(uNormalTexture, vTexCoord).rgb * 2.0 - 1.0;
		normal = normalize(mat3(tangent, bitangent, normal) * tangentNormal);
	}

	if (!gl_FrontFacing) {
		normal = -normal;
	}

	float roughness = uRoughness;
	if (uHasRoughnessTexture) {
		roughness *= texture(uRoughnessTexture, vTexCoord).r;
	}

	float metallic = uMetallic;
	if (uHasMetallicTexture) {
		metallic *= texture(uMetallicTexture, vTexCoord).r;
	}

	roughness = clamp(roughness, 0.04, 1.0);
	metallic = clamp(metallic, 0.0, 1.0);

	vec3 viewDirection = normalize(uViewPosition - vWorldPosition);
	vec3 illumination = baseColor.rgb * uAmbientColor * uAmbientIntensity * (1.0 - metallic);

	if (uHasCubeMap) {
		vec3 reflectionDirection = reflect(-viewDirection, normal);
		float maxMipLevel = floor(log2(float(textureSize(uCubeMap, 0).x)));
		vec3 environment = textureLod(uCubeMap, reflectionDirection, roughness * maxMipLevel).rgb;
		vec3 f0 = mix(vec3(0.04), baseColor.rgb, metallic);
		vec3 fresnel = fresnelSchlick(max(dot(normal, viewDirection), 0.0), f0);

		illumination += environment * fresnel;
	}

	for (int i = 0; i < uDirectionalLightCount; ++i) {
		DirectionalLight light = uDirectionalLights[i];
		vec3 towardLight = -normalize(light.direction);

		illumination += evaluateLight(normal, viewDirection, towardLight, light.color * light.intensity, baseColor.rgb, metallic, roughness);
	}

	for (int i = 0; i < uPointLightCount; ++i) {
		PointLight light = uPointLights[i];
		vec3 towardLight;
		float falloff = localLightFalloff(light.position - vWorldPosition, light.range, towardLight);

		illumination += evaluateLight(normal, viewDirection, towardLight, light.color * light.intensity * falloff, baseColor.rgb, metallic, roughness);
	}

	for (int i = 0; i < uSpotLightCount; ++i) {
		SpotLight light = uSpotLights[i];
		vec3 towardLight;
		float falloff = localLightFalloff(light.position - vWorldPosition, light.range, towardLight);
		float coneCos = dot(light.direction, -towardLight);
		float cone = smoothstep(light.outerConeCos, light.innerConeCos, coneCos);

		illumination += evaluateLight(normal, viewDirection, towardLight, light.color * light.intensity * falloff * cone, baseColor.rgb, metallic, roughness);
	}

	fragmentColor = vec4(illumination, baseColor.a);
}