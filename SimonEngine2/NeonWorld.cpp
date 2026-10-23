#include "NeonWorld.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>

#include "EngineMath.hpp"

namespace NeonWorld {

	using GamerTheme::Rgb;

	namespace {
		constexpr int gridSegments = 32;      // each line is chopped up so it can fade per vertex
		constexpr float gridScrollSpeed = 0.35f; // world units per second ("we're moving!!")

		GLuint overrideTexture = 0;

		// Set per drawFloor() call: the grid fades to nothing at gridExtent,
		// with gridSpacing world units between lines
		float gridExtent = 8.0f;
		float gridSpacing = 0.5f;

		// Grid brightness at a given distance from the center: full in the middle, easing
		// out to zero at the edge. Squared so the falloff feels like fog, not a hard cutoff.
		float gridFade(float x, float z) {
			const float d = std::sqrt(x * x + z * z) / gridExtent;
			const float f = std::max(0.0f, 1.0f - d);
			return f * f;
		}

		void gridVertex(const Rgb& c, float alpha, float x, float y, float z) {
			glColor4f(c.r, c.g, c.b, alpha * gridFade(x, z));
			glVertex3f(x, y, z);
		}

		void drawGridLines(const Rgb& c, float alpha, float floorY, float scroll) {
			const int lines = static_cast<int>(gridExtent / gridSpacing);
			const float step = (2.0f * gridExtent) / gridSegments;

			glBegin(GL_LINES);
			for (int i = -lines; i <= lines; ++i) {
				// Lines running along Z (fixed x) stay put...
				const float x = i * gridSpacing;
				for (int s = 0; s < gridSegments; ++s) {
					const float z0 = -gridExtent + s * step;
					gridVertex(c, alpha, x, floorY, z0);
					gridVertex(c, alpha, x, floorY, z0 + step);
				}

				// ...while lines running along X (fixed z) scroll toward the camera, which is
				// what sells the "the arena is flying past us" motion
				const float z = i * gridSpacing + scroll;
				for (int s = 0; s < gridSegments; ++s) {
					const float x0 = -gridExtent + s * step;
					gridVertex(c, alpha, x0, floorY, z);
					gridVertex(c, alpha, x0 + step, floorY, z);
				}
			}
			glEnd();
		}

		void floorDisk(float y, float radius, const Rgb& inner, float innerAlpha, const Rgb& outer, float outerAlpha) {
			const int segments = 48;
			glBegin(GL_TRIANGLE_FAN);
			glColor4f(inner.r, inner.g, inner.b, innerAlpha);
			glVertex3f(0.0f, y, 0.0f);
			glColor4f(outer.r, outer.g, outer.b, outerAlpha);
			for (int i = 0; i <= segments; ++i) {
				const float a = 2.0f * EngineMath::PI * i / segments;
				glVertex3f(std::cos(a) * radius, y, std::sin(a) * radius);
			}
			glEnd();
		}

		void floorRing(float y, float radius, const Rgb& c, float alpha, float lineWidth, int dashes, float rotation) {
			glLineWidth(lineWidth);
			glColor4f(c.r, c.g, c.b, alpha);

			if (dashes <= 0) {
				glBegin(GL_LINE_LOOP);
				for (int i = 0; i < 64; ++i) {
					const float a = 2.0f * EngineMath::PI * i / 64.0f;
					glVertex3f(std::cos(a) * radius, y, std::sin(a) * radius);
				}
				glEnd();
				return;
			}

			// Dashed ring: each dash covers 60% of its slot, the rest is a gap
			glBegin(GL_LINES);
			const float slot = 2.0f * EngineMath::PI / dashes;
			for (int d = 0; d < dashes; ++d) {
				const float a0 = rotation + d * slot;
				const float a1 = a0 + slot * 0.6f;
				const int pieces = 4;
				for (int p = 0; p < pieces; ++p) {
					const float s0 = a0 + (a1 - a0) * p / pieces;
					const float s1 = a0 + (a1 - a0) * (p + 1) / pieces;
					glVertex3f(std::cos(s0) * radius, y, std::sin(s0) * radius);
					glVertex3f(std::cos(s1) * radius, y, std::sin(s1) * radius);
				}
			}
			glEnd();
		}

		void ensureOverrideTexture() {
			if (overrideTexture == 0) {
				glGenTextures(1, &overrideTexture);
			}
		}
	}

	// ======================================================================== floor

	void drawFloor(const GamerTheme::Theme& p, float t, float floorY, float padRadius, float extent, float spacing) {
		gridExtent = extent;
		gridSpacing = spacing;

		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);

		// Depth TEST on (objects hide the floor behind them), depth WRITE off (the floor is
		// see-through glow, it must never block anything drawn after it)
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);

		// ---- contact shadow: normal blending, darkens whatever is underneath
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		const Rgb black = { 0.0f, 0.0f, 0.0f };
		floorDisk(floorY + 0.001f, padRadius * 1.35f, black, 0.6f, black, 0.0f);

		// ---- everything else glows: additive blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		const float scroll = std::fmod(t * gridScrollSpeed, gridSpacing);
		glLineWidth(3.0f);
		drawGridLines(p.grid, 0.16f, floorY, scroll); // wide faint pass = glow
		glLineWidth(1.0f);
		drawGridLines(GamerTheme::mixWhite(p.grid, 0.25f), 0.55f, floorY, scroll); // thin bright core

		// ---- spawn pad: soft colored pool of light + pulsing ring + spinning dashes
		const float pulse = 0.5f + 0.5f * std::sin(t * 2.4f);
		floorDisk(floorY + 0.002f, padRadius * 1.2f, p.accent, 0.10f + 0.06f * pulse, p.accent, 0.0f);

		const float ringY = floorY + 0.003f;
		floorRing(ringY, padRadius, p.accent, 0.20f, 5.0f, 0, 0.0f);
		floorRing(ringY, padRadius, GamerTheme::mixWhite(p.accent, 0.4f), 0.55f + 0.35f * pulse, 1.5f, 0, 0.0f);
		floorRing(ringY, padRadius * 1.18f, p.accent2, 0.7f, 2.0f, 12, t * 0.6f);
		floorRing(ringY, padRadius * 1.30f, p.accent2, 0.35f, 1.0f, 24, -t * 0.35f);

		// A shockwave ring that keeps expanding outward from the pad and fading away
		const float wave = std::fmod(t * 0.45f, 1.0f);
		floorRing(ringY, padRadius * (1.0f + wave * 2.5f), p.accent, 0.5f * (1.0f - wave) * (1.0f - wave), 2.0f, 0, 0.0f);

		glPopAttrib();
	}

	// ======================================================================== outlines

	int outlinePassCount() {
		return 2;
	}

	void beginOutlinePass(const GamerTheme::Theme& p, int pass) {
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		// -------------------------------------------------------------------------------
		//  THE COLOR OVERRIDE HACK (aka "how to recolor code you're not allowed to touch")
		//
		//  Every primitive calls glColor3f(its own color) right before drawing, and every
		//  one of them calls glDisable(GL_TEXTURE_2D) first. None of them know or care about
		//  GL_TEXTURE_1D. So:
		//
		//    1. make a 1D texture that is exactly ONE texel big, holding the neon color
		//    2. enable GL_TEXTURE_1D (which they never turn off)
		//    3. set the texture environment to GL_REPLACE = "ignore the vertex color,
		//       the texture's color wins"
		//
		//  No texture coordinates are ever sent, so every vertex samples texel 0, and that
		//  one texel is our color. Result: the entire scene renders in neon, alpha included,
		//  without changing a single line of the primitives. Pure OpenGL 1.1, works on the
		//  oldest drivers Windows ships.
		// -------------------------------------------------------------------------------
		const Rgb color = pass == 0 ? p.accent : GamerTheme::mixWhite(p.accent, 0.45f);
		const float alpha = pass == 0 ? 0.22f : 0.80f;

		const GLubyte texel[4] = {
			static_cast<GLubyte>(std::min(1.0f, color.r) * 255.0f),
			static_cast<GLubyte>(std::min(1.0f, color.g) * 255.0f),
			static_cast<GLubyte>(std::min(1.0f, color.b) * 255.0f),
			static_cast<GLubyte>(alpha * 255.0f)
		};

		ensureOverrideTexture();
		glBindTexture(GL_TEXTURE_1D, overrideTexture);
		// The default minification filter wants mipmaps; without mipmaps the texture counts
		// as "incomplete" and OpenGL silently disables texturing. NEAREST avoids all that.
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texel);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glEnable(GL_TEXTURE_1D);

		glDisable(GL_LIGHTING);

		// Draw every polygon as its outline...
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// ...pulled slightly toward the camera so the lines win the depth test against the
		// solid surfaces they sit on (otherwise you get z-fighting shimmer)
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, -2.0f);

		// Hidden edges stay hidden (depth test), but the glow never blocks anything
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		if (pass == 0) {
			glLineWidth(5.0f); // fat + faint = the glow
		}
		else {
			glLineWidth(1.5f); // thin + bright = the neon tube
			glEnable(GL_LINE_SMOOTH);
			glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		}
	}

	void endOutlinePass() {
		glPopAttrib();
	}
}
