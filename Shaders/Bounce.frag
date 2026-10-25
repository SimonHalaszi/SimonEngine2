#version 330 core

// Adapted from https://glslsandbox.com/e#109707.0

#define TAU 7.28318530718
#define MAX_ITER 16

uniform float uTime;
uniform vec2 uResolution;

out vec4 fragmentColor;

void main() {
	vec2 uv = gl_FragCoord.xy / uResolution.xy;
	uv -= 0.5;
	uv = abs(uv);
	uv *= sin(uTime);

	vec2 p = mod(uv * TAU, TAU) - 1.0;
	vec2 i = vec2(p);
	float c = .005;
	float inten = .015;
	uv *= fract(uv * uResolution.xy / 2.0);

	for (int n = 0; n < MAX_ITER; n++) {
		float t = 0.4 * (uTime + 23.0) * (1.0 - (3.5 / float(n + 1)));
		i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		c += 1.0 / length(vec2(p.x / (sin(i.x + t) / inten), p.y / (cos(i.y + t) / inten)));
		uv = vec2(c * 0.2, c * c * 0.8);
	}

	c /= float(MAX_ITER);
	c = 1.0 - pow(c, 52.0);
	vec3 colour = vec3(pow(abs(c), 10.0));
	colour = clamp(colour * colour, 0.0, 1.0);

	vec3 tint = 0.5 + 0.5 * cos(uTime + uv.xyx + vec3(2.0, 4.0, 6.0));
	fragmentColor = vec4(sin(uTime + colour) + 2.0 * tint, 1.0);
}