#include "GamerTheme.hpp"

#include <cmath>

namespace GamerTheme {

	namespace {
		// -----------------------------------------------------------------------------
		//  THE PALETTE VAULT
		//  Every color here was picked by staring at gaming peripherals for way too long.
		//  Order = F2 cycle order. The first one is what you boot into.
		// -----------------------------------------------------------------------------
		const Theme themes[] = {
			{
				"SYNTHWAVE",                       // outrun / miami nights / 1986 forever
				{ 0.00f, 0.95f, 1.00f },           // accent: electric cyan
				{ 1.00f, 0.20f, 0.75f },           // accent2: hot magenta
				{ 0.02f, 0.00f, 0.07f },           // sky top: deep space purple
				{ 0.30f, 0.03f, 0.32f },           // sky horizon: sunset violet
				{ 1.00f, 0.88f, 0.25f },           // sun top: golden
				{ 1.00f, 0.13f, 0.55f },           // sun bottom: pink
				{ 1.00f, 0.20f, 0.80f },           // grid: magenta
				false
			},
			{
				"VENOM",                           // that one snake-branded mouse you own
				{ 0.27f, 1.00f, 0.25f },
				{ 0.00f, 0.75f, 0.45f },
				{ 0.00f, 0.02f, 0.01f },
				{ 0.02f, 0.16f, 0.06f },
				{ 0.75f, 1.00f, 0.35f },
				{ 0.00f, 0.45f, 0.15f },
				{ 0.20f, 1.00f, 0.30f },
				false
			},
			{
				"INFERNO",                         // thermal paste optional
				{ 1.00f, 0.50f, 0.05f },
				{ 1.00f, 0.08f, 0.12f },
				{ 0.04f, 0.00f, 0.00f },
				{ 0.32f, 0.05f, 0.01f },
				{ 1.00f, 0.92f, 0.35f },
				{ 1.00f, 0.12f, 0.00f },
				{ 1.00f, 0.30f, 0.05f },
				false
			},
			{
				"SUBZERO",                         // for when your GPU runs at 34C
				{ 0.55f, 0.88f, 1.00f },
				{ 0.35f, 0.40f, 1.00f },
				{ 0.00f, 0.01f, 0.05f },
				{ 0.04f, 0.14f, 0.30f },
				{ 0.92f, 0.98f, 1.00f },
				{ 0.30f, 0.55f, 1.00f },
				{ 0.40f, 0.80f, 1.00f },
				false
			},
			{
				"VAPORWAVE",                       // A E S T H E T I C
				{ 1.00f, 0.45f, 0.85f },
				{ 0.30f, 1.00f, 0.90f },
				{ 0.06f, 0.02f, 0.14f },
				{ 0.42f, 0.14f, 0.45f },
				{ 1.00f, 0.95f, 0.55f },
				{ 1.00f, 0.42f, 0.72f },
				{ 0.35f, 1.00f, 0.95f },
				false
			},
			{
				"RGB OVERDRIVE",                   // the accents below are ignored, it's all rainbow
				{ 1.00f, 0.00f, 0.00f },
				{ 0.00f, 0.00f, 1.00f },
				{ 0.01f, 0.01f, 0.03f },
				{ 0.12f, 0.06f, 0.20f },
				{ 1.00f, 1.00f, 1.00f },
				{ 0.60f, 0.20f, 1.00f },
				{ 1.00f, 1.00f, 1.00f },
				true
			},
		};

		const int themeTotal = static_cast<int>(sizeof(themes) / sizeof(themes[0]));

		// Unlocked with up up down down left right left right B A. Gold + royal purple,
		// with the rainbow cranked on top because subtlety is for console players.
		const Theme chuckMode = {
			"CHUCK MODE",
			{ 1.00f, 0.80f, 0.10f },
			{ 0.65f, 0.20f, 1.00f },
			{ 0.03f, 0.00f, 0.06f },
			{ 0.35f, 0.12f, 0.05f },
			{ 1.00f, 0.95f, 0.40f },
			{ 0.85f, 0.25f, 1.00f },
			{ 1.00f, 0.80f, 0.20f },
			true
		};

		float wrap01(float v) {
			v = std::fmod(v, 1.0f);
			return v < 0.0f ? v + 1.0f : v;
		}
	}

	int themeCount() {
		return themeTotal;
	}

	const Theme& theme(int index) {
		index %= themeTotal;
		if (index < 0) {
			index += themeTotal;
		}
		return themes[index];
	}

	const Theme& chuckModeTheme() {
		return chuckMode;
	}

	Rgb lerp(const Rgb& a, const Rgb& b, float t) {
		return { a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t };
	}

	Rgb scale(const Rgb& c, float s) {
		return { c.r * s, c.g * s, c.b * s };
	}

	Rgb mixWhite(const Rgb& c, float amount) {
		return lerp(c, Rgb{ 1.0f, 1.0f, 1.0f }, amount);
	}

	// Classic six-sector HSV conversion. The hue wheel is split into six 60-degree wedges,
	// and inside each wedge one channel ramps while the other two stay pinned.
	Rgb hsv(float hue, float saturation, float value) {
		const float h = wrap01(hue) * 6.0f;
		const int sector = static_cast<int>(h) % 6;
		const float f = h - std::floor(h);

		const float p = value * (1.0f - saturation);
		const float q = value * (1.0f - saturation * f);
		const float t = value * (1.0f - saturation * (1.0f - f));

		switch (sector) {
			case 0:  return { value, t, p };
			case 1:  return { q, value, p };
			case 2:  return { p, value, t };
			case 3:  return { p, q, value };
			case 4:  return { t, p, value };
			default: return { value, p, q };
		}
	}

	Theme blend(const Theme& from, const Theme& to, float t) {
		Theme out = t < 0.5f ? from : to; // name + rainbow flag snap at the halfway point
		out.accent = lerp(from.accent, to.accent, t);
		out.accent2 = lerp(from.accent2, to.accent2, t);
		out.skyTop = lerp(from.skyTop, to.skyTop, t);
		out.skyHorizon = lerp(from.skyHorizon, to.skyHorizon, t);
		out.sunTop = lerp(from.sunTop, to.sunTop, t);
		out.sunBottom = lerp(from.sunBottom, to.sunBottom, t);
		out.grid = lerp(from.grid, to.grid, t);
		return out;
	}

	Theme resolve(const Theme& base, float timeSeconds) {
		if (!base.rainbow) {
			return base;
		}

		// One full trip around the hue wheel every ~7 seconds. accent2 rides the opposite
		// side of the wheel so the two neons always contrast. The grid lags a little behind
		// the accent so the floor looks like a wave of color rolling across it.
		Theme out = base;
		const float hue = timeSeconds * 0.14f;
		out.accent = hsv(hue, 0.85f, 1.0f);
		out.accent2 = hsv(hue + 0.5f, 0.85f, 1.0f);
		out.grid = hsv(hue - 0.08f, 0.75f, 1.0f);
		out.sunBottom = lerp(base.sunBottom, hsv(hue + 0.25f, 0.9f, 1.0f), 0.6f);
		out.skyHorizon = lerp(base.skyHorizon, scale(hsv(hue + 0.6f, 0.8f, 1.0f), 0.3f), 0.5f);
		return out;
	}
}
