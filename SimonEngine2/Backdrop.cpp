#include "Backdrop.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>

#include "Render2D.hpp"

namespace Backdrop {

	using GamerTheme::Rgb;

	namespace {
		constexpr int starCount = 140;
		constexpr int emberCount = 70;
		constexpr int foregroundEmberCount = 14;
		constexpr int buildingCount = 34;

		// -----------------------------------------------------------------------------
		//  Deterministic "random" numbers: the same (index, salt) always gives the same
		//  value in [0, 1). This is an integer hash (multiply, xor-shift, repeat), which is
		//  how we get 140 stars that stay put frame after frame without storing any of them.
		// -----------------------------------------------------------------------------
		float hash01(unsigned int index, unsigned int salt) {
			unsigned int x = index * 374761393u + salt * 668265263u + 0x9E3779B9u;
			x = (x ^ (x >> 13)) * 1274126177u;
			x ^= x >> 16;
			return static_cast<float>(x & 0x00FFFFFFu) / 16777216.0f;
		}

		float sunHalfWidth(float y, float cy, float r) {
			const float dy = y - cy;
			return std::sqrt(std::max(0.0f, r * r - dy * dy));
		}

		void drawSky(const GamerTheme::Theme& p, float w, float h, float horizonY) {
			// Sky: top -> horizon, then the ground gets its own gradient later
			Render2D::rectGradientV(0.0f, 0.0f, w, horizonY, p.skyTop, 1.0f, p.skyHorizon, 1.0f);
		}

		void drawStars(const GamerTheme::Theme& p, float w, float horizonY, float t) {
			glPointSize(1.0f);
			glBegin(GL_POINTS);
			for (unsigned int i = 0; i < starCount; ++i) {
				const float x = hash01(i, 1) * w;
				const float y = hash01(i, 2) * horizonY * 0.95f;
				if (hash01(i, 6) > 0.8f) {
					continue; // the bright ones get drawn bigger below
				}
				const float twinkle = 0.5f + 0.5f * std::sin(t * (0.8f + hash01(i, 3) * 2.5f) + hash01(i, 4) * 6.283f);
				// stars fade out as they approach the glowing horizon
				const float fade = 1.0f - (y / horizonY);
				const float a = (0.25f + 0.75f * twinkle) * fade * (0.4f + 0.6f * hash01(i, 5));
				glColor4f(0.9f, 0.92f, 1.0f, a);
				glVertex2f(x, y);
			}
			glEnd();

			// The ~20% "bright" stars get a 2px point plus a tint from the accent color
			glPointSize(2.0f);
			glBegin(GL_POINTS);
			for (unsigned int i = 0; i < starCount; ++i) {
				if (hash01(i, 6) <= 0.8f) {
					continue;
				}
				const float x = hash01(i, 1) * w;
				const float y = hash01(i, 2) * horizonY * 0.95f;
				const float twinkle = 0.5f + 0.5f * std::sin(t * (0.6f + hash01(i, 3) * 1.5f) + hash01(i, 4) * 6.283f);
				const Rgb c = GamerTheme::mixWhite(p.accent, 0.6f);
				glColor4f(c.r, c.g, c.b, (0.3f + 0.7f * twinkle) * (1.0f - y / horizonY));
				glVertex2f(x, y);
			}
			glEnd();
			glPointSize(1.0f);
		}

		void drawSun(const GamerTheme::Theme& p, float w, float h, float horizonY, float t) {
			// Big enough to peek out on both sides of whatever hero is standing in front of it,
			// but never taller than the sky above the horizon (perspective cameras can push
			// the horizon way up the screen)
			const float r = std::min(std::min(w, h) * 0.27f, horizonY * 0.85f);
			const float cx = w * 0.5f;
			const float cy = horizonY - r * 0.50f; // sitting low, half sunk behind the skyline

			// Big soft glow behind the sun (additive = it brightens whatever sky is behind it)
			Render2D::blendAdditive();
			Render2D::radialGlow(cx, cy, r * 2.3f, p.sunBottom, 0.30f, p.sunBottom, 0.0f, 64);
			Render2D::radialGlow(cx, cy, r * 1.35f, p.sunTop, 0.18f, p.sunTop, 0.0f, 64);
			Render2D::blendNormal();

			// ---------------------------------------------------------------------------
			//  The sun body is drawn as thin horizontal slices from top to bottom, each one
			//  a trapezoid fitted to the circle. The bottom half gets the iconic retro
			//  stripes: gaps that get THICKER toward the bottom and slowly scroll downward.
			// ---------------------------------------------------------------------------
			const int slices = 90;
			const float top = cy - r;
			const float sliceH = (2.0f * r) / slices;
			const float stripePeriod = r * 0.17f;
			const float scroll = std::fmod(t * r * 0.04f, stripePeriod);

			glBegin(GL_QUADS);
			for (int i = 0; i < slices; ++i) {
				const float y0 = top + i * sliceH;
				const float y1 = y0 + sliceH;
				if (y0 >= horizonY) {
					break; // below the horizon = hidden behind the ground
				}

				const float mid = 0.5f * (y0 + y1);
				const float stripeZone = (mid - (cy - r * 0.05f)) / (r * 1.05f); // 0 at the stripe start, 1 at the bottom
				if (stripeZone > 0.0f) {
					const float gapFraction = 0.10f + 0.50f * stripeZone;
					const float phase = std::fmod(mid - cy + scroll, stripePeriod);
					if (phase < gapFraction * stripePeriod) {
						continue; // this slice is part of a gap stripe
					}
				}

				const float k = static_cast<float>(i) / slices;
				const Rgb c = GamerTheme::lerp(p.sunTop, p.sunBottom, k);
				const float hw0 = sunHalfWidth(y0, cy, r);
				const float hw1 = sunHalfWidth(y1, cy, r);

				glColor4f(c.r, c.g, c.b, 1.0f);
				glVertex2f(cx - hw0, y0);
				glVertex2f(cx + hw0, y0);
				glVertex2f(cx + hw1, y1);
				glVertex2f(cx - hw1, y1);
			}
			glEnd();
		}

		void drawSkyline(const GamerTheme::Theme& p, float w, float horizonY, float t) {
			// Silhouettes a bit darker than the horizon sky so they read as "far away"
			const Rgb silhouette = GamerTheme::lerp(p.skyTop, p.skyHorizon, 0.25f);
			const float unit = w / buildingCount;

			for (unsigned int i = 0; i < buildingCount; ++i) {
				const float bw = unit * (0.7f + hash01(i, 20) * 0.9f);
				const float bx = i * unit - bw * 0.2f;
				// Taller buildings toward the edges, low in the middle so the sun stays visible
				const float edge = std::fabs((bx + bw * 0.5f) / w - 0.5f) * 2.0f;
				const float bh = (12.0f + hash01(i, 21) * 50.0f) * (0.35f + edge * 1.3f);
				Render2D::rect(bx, horizonY - bh, bw, bh + 1.0f, silhouette, 1.0f);

				// Little antenna on some of the tall ones, with a blinking aircraft light
				if (bh > 45.0f && hash01(i, 22) > 0.5f) {
					const float ax = bx + bw * 0.5f;
					Render2D::rect(ax - 0.5f, horizonY - bh - 10.0f, 1.0f, 10.0f, silhouette, 1.0f);
					const float blink = std::sin(t * 2.2f + hash01(i, 23) * 6.0f) > 0.6f ? 1.0f : 0.15f;
					Render2D::rect(ax - 1.0f, horizonY - bh - 12.0f, 2.0f, 2.0f, p.accent2, blink);
				}

				// Lit windows: a sparse grid of 2x2 dots in the accent colors
				const int cols = std::max(1, static_cast<int>(bw / 6.0f));
				const int rows = std::max(1, static_cast<int>(bh / 7.0f));
				for (int cx = 0; cx < cols; ++cx) {
					for (int cy = 0; cy < rows; ++cy) {
						const unsigned int id = i * 977u + cx * 31u + cy;
						if (hash01(id, 24) > 0.18f) {
							continue;
						}
						const Rgb wc = hash01(id, 25) > 0.5f ? p.accent : p.accent2;
						const float flicker = 0.55f + 0.45f * std::sin(t * 0.7f + hash01(id, 26) * 40.0f);
						Render2D::rect(bx + 2.0f + cx * 6.0f, horizonY - bh + 3.0f + cy * 7.0f, 2.0f, 2.0f, wc, 0.55f * flicker);
					}
				}
			}

			// The bright horizon line where the city meets the ground
			Render2D::blendAdditive();
			Render2D::rectGradientV(0.0f, horizonY - 6.0f, w, 6.0f, p.sunBottom, 0.0f, p.sunBottom, 0.35f);
			Render2D::rect(0.0f, horizonY, w, 1.0f, GamerTheme::mixWhite(p.sunBottom, 0.3f), 0.55f);
			Render2D::blendNormal();
		}

		void drawGround(const GamerTheme::Theme& p, float w, float h, float horizonY) {
			// (not named near/far: <windows.h> #defines both of those to nothing, lol)
			const Rgb closeColor = GamerTheme::scale(p.skyTop, 0.6f);
			const Rgb distantColor = GamerTheme::lerp(p.skyTop, p.skyHorizon, 0.45f);
			Render2D::rectGradientV(0.0f, horizonY + 1.0f, w, h - horizonY, distantColor, 1.0f, closeColor, 1.0f);
		}

		// One ember. Everything about it is derived from its index + the clock:
		// it rises from the bottom, wraps to the bottom again, and sways sideways.
		void drawEmber(unsigned int i, unsigned int salt, const GamerTheme::Theme& p, float w, float h, float t, float sizeScale, float alphaScale) {
			const float speed = 0.025f + hash01(i, salt + 1) * 0.06f;              // screens per second
			const float life = std::fmod(hash01(i, salt + 2) + t * speed, 1.0f);   // 0 = bottom, 1 = top
			const float y = h * (1.05f - life * 1.1f);
			const float x = hash01(i, salt + 3) * w + std::sin(t * (0.5f + hash01(i, salt + 4)) + i) * 18.0f;
			const float size = (1.2f + hash01(i, salt + 5) * 2.3f) * sizeScale;

			// fade in at the bottom, fade out near the top, plus a little flicker
			const float fade = std::min(1.0f, life * 6.0f) * std::min(1.0f, (1.0f - life) * 3.0f);
			const float flicker = 0.7f + 0.3f * std::sin(t * 9.0f + i * 1.7f);
			const Rgb c = (i % 3 == 0) ? p.accent2 : p.accent;

			Render2D::radialGlow(x, y, size * 3.0f, c, 0.35f * fade * flicker * alphaScale, c, 0.0f, 12);
			Render2D::rect(x - size * 0.5f, y - size * 0.5f, size, size, GamerTheme::mixWhite(c, 0.5f), 0.8f * fade * flicker * alphaScale);
		}
	}

	void draw(const GamerTheme::Theme& palette, float t, float horizonFraction) {
		Render2D::begin();

		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());
		const float horizonY = h * horizonFraction;

		drawSky(palette, w, h, horizonY);
		drawStars(palette, w, horizonY, t);
		drawSun(palette, w, h, horizonY, t);
		drawSkyline(palette, w, horizonY, t);
		drawGround(palette, w, h, horizonY);

		Render2D::blendAdditive();
		for (unsigned int i = 0; i < emberCount; ++i) {
			drawEmber(i, 100, palette, w, h, t, 1.0f, 1.0f);
		}
		Render2D::blendNormal();

		Render2D::end();
	}

	void drawForegroundEmbers(const GamerTheme::Theme& palette, float t) {
		// Called inside an existing Render2D::begin()/end() block (the overlay pass)
		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());

		Render2D::blendAdditive();
		for (unsigned int i = 0; i < foregroundEmberCount; ++i) {
			// Bigger + dimmer + faster = reads as "closer to the camera"
			drawEmber(i, 500, palette, w, h, t * 1.6f, 2.6f, 0.45f);
		}
		Render2D::blendNormal();
	}
}
