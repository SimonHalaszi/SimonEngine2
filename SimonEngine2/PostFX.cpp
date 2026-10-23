#include "PostFX.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>

#include "Render2D.hpp"

namespace PostFX {

	using GamerTheme::Rgb;

	namespace {
		const Rgb black = { 0.0f, 0.0f, 0.0f };
		const Rgb white = { 1.0f, 1.0f, 1.0f };

		// Same integer hash trick as the backdrop: deterministic "random" from a number
		float hash01(unsigned int x) {
			x = (x ^ 61u) ^ (x >> 16);
			x *= 9u;
			x ^= x >> 4;
			x *= 0x27d4eb2du;
			x ^= x >> 15;
			return static_cast<float>(x & 0xFFFFu) / 65535.0f;
		}
	}

	void vignette(float strength) {
		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());
		const float band = std::min(w, h) * 0.22f;

		// Four gradients from the edges inward. Where two overlap (the corners) it gets
		// twice as dark, which is exactly what a real lens vignette does anyway.
		Render2D::rectGradientV(0.0f, 0.0f, w, band, black, strength, black, 0.0f);
		Render2D::rectGradientV(0.0f, h - band, w, band, black, 0.0f, black, strength);
		Render2D::rectGradientH(0.0f, 0.0f, band, h, black, strength, black, 0.0f);
		Render2D::rectGradientH(w - band, 0.0f, band, h, black, 0.0f, black, strength);
	}

	void letterbox(float amount) {
		if (amount <= 0.001f) {
			return;
		}
		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());

		// 2.39:1 is the "this is a movie now" aspect ratio. Work out how tall the bars must
		// be to crop the window to it, then slide them in by `amount`.
		const float targetH = w / 2.39f;
		const float bar = std::max(0.0f, (h - targetH) * 0.5f) * amount;
		Render2D::rect(0.0f, 0.0f, w, bar, black, 1.0f);
		Render2D::rect(0.0f, h - bar, w, bar, black, 1.0f);
	}

	void crt(const GamerTheme::Theme& p, float t) {
		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());

		// ---- scanlines: a dark line every 3 pixels
		glLineWidth(1.0f);
		glColor4f(0.0f, 0.0f, 0.0f, 0.24f);
		glBegin(GL_LINES);
		for (float y = 0.5f; y < h; y += 3.0f) {
			glVertex2f(0.0f, y);
			glVertex2f(w, y);
		}
		glEnd();

		// ---- aperture grille: faint red / green / blue vertical stripes, like the phosphor
		// columns on a Trinitron. Additive, so they tint instead of darken.
		Render2D::blendAdditive();
		glBegin(GL_LINES);
		for (int x = 0; x < static_cast<int>(w); ++x) {
			const int channel = x % 3;
			glColor4f(channel == 0 ? 1.0f : 0.0f, channel == 1 ? 1.0f : 0.0f, channel == 2 ? 1.0f : 0.0f, 0.035f);
			glVertex2f(x + 0.5f, 0.0f);
			glVertex2f(x + 0.5f, h);
		}
		glEnd();

		// ---- the rolling bright band that crawls down the screen
		const float bandH = h * 0.12f;
		const float bandY = std::fmod(t * 70.0f, h + bandH) - bandH;
		Render2D::rectGradientV(0.0f, bandY, w, bandH * 0.5f, white, 0.0f, white, 0.045f);
		Render2D::rectGradientV(0.0f, bandY + bandH * 0.5f, w, bandH * 0.5f, white, 0.045f, white, 0.0f);

		// ---- glitch tears: roughly every 2.7 seconds, for ~0.12s, a few horizontal slices
		// light up in the theme colors at "random" heights
		const float cycle = 2.7f;
		const unsigned int burst = static_cast<unsigned int>(t / cycle);
		const float phase = std::fmod(t, cycle);
		if (phase < 0.12f) {
			for (unsigned int i = 0; i < 4; ++i) {
				const float y = hash01(burst * 13u + i) * h;
				const float sliceH = 2.0f + hash01(burst * 31u + i) * 10.0f;
				const float offset = (hash01(burst * 7u + i) - 0.5f) * 60.0f;
				const Rgb c = (i % 2) ? p.accent : p.accent2;
				Render2D::rect(offset, y, w, sliceH, c, 0.22f);
			}
		}
		Render2D::blendNormal();

		// ---- 60 Hz flicker + a heavier vignette (CRT glass is curved)
		const float flicker = 0.025f * (0.5f + 0.5f * std::sin(t * 377.0f));
		Render2D::rect(0.0f, 0.0f, w, h, black, flicker);
		vignette(0.35f);
	}

	void flash(float alpha) {
		if (alpha <= 0.0f) {
			return;
		}
		Render2D::rect(0.0f, 0.0f, static_cast<float>(Render2D::width()), static_cast<float>(Render2D::height()), white, alpha);
	}
}
