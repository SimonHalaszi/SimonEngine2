#include "Render2D.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>

#include "EngineMath.hpp"

namespace Render2D {

	namespace {
		int viewWidth = 1;
		int viewHeight = 1;

		// GLUT stroke Roman capitals are ~100 font units tall. Divide by this to turn
		// "I want 24px tall letters" into a glScale factor.
		constexpr float strokeCapUnits = 100.0f;

		void* bitmapFont(Font font) {
			switch (font) {
				case Font::Tiny:      return GLUT_BITMAP_HELVETICA_10;
				case Font::Body:      return GLUT_BITMAP_HELVETICA_12;
				case Font::Large:     return GLUT_BITMAP_HELVETICA_18;
				case Font::Mono:      return GLUT_BITMAP_9_BY_15;
				case Font::MonoSmall: return GLUT_BITMAP_8_BY_13;
			}
			return GLUT_BITMAP_HELVETICA_12;
		}

		void setColor(const Rgb& color, float alpha) {
			glColor4f(color.r, color.g, color.b, alpha);
		}

		// -----------------------------------------------------------------------------
		//  THE RASTER POSITION TRICK
		//  Bitmap text starts at the "raster position". If you glRasterPos() to a point even
		//  ONE pixel outside the window, OpenGL marks the raster position invalid and the
		//  entire string silently vanishes. That's why text sliding in from the edge of the
		//  screen normally pops in instead of sliding.
		//
		//  Fix: put the raster position at the nearest point that IS on screen, then nudge it
		//  to where we actually want it with an empty glBitmap() call. glBitmap's "move"
		//  offsets don't get validated, so the position stays valid even off-screen and the
		//  glyphs simply get clipped pixel by pixel like you'd expect.
		//  (glBitmap's offsets are in window coordinates, which point y UP, hence the minus.)
		// -----------------------------------------------------------------------------
		void rasterAt(float x, float y) {
			const float safeX = std::min(std::max(x, 0.0f), static_cast<float>(viewWidth - 1));
			const float safeY = std::min(std::max(y, 1.0f), static_cast<float>(viewHeight - 1));

			glRasterPos2f(std::floor(safeX), std::floor(safeY));
			glBitmap(0, 0, 0.0f, 0.0f, std::floor(x) - std::floor(safeX), -(std::floor(y) - std::floor(safeY)), nullptr);
		}
	}

	// ======================================================================== frame setup

	void begin() {
		// Snapshot EVERYTHING. The 3D world leaves lighting, depth testing, line widths,
		// polygon modes and a sneaky 1D texture enabled at various points; none of it is
		// allowed to bleed into the HUD, and none of the HUD's state may bleed back out.
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		viewWidth = std::max(1, glutGet(GLUT_WINDOW_WIDTH));
		viewHeight = std::max(1, glutGet(GLUT_WINDOW_HEIGHT));

		glViewport(0, 0, viewWidth, viewHeight);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		// left, right, BOTTOM = height, TOP = 0  ->  y grows downward, like a HUD artist expects
		glOrtho(0.0, viewWidth, viewHeight, 0.0, -1.0, 1.0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_1D);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
		glDisable(GL_FOG);
		glDisable(GL_POLYGON_OFFSET_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_BLEND);
		blendNormal();

		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}

	void end() {
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glPopAttrib();
	}

	int width() { return viewWidth; }
	int height() { return viewHeight; }

	void blendAdditive() { glBlendFunc(GL_SRC_ALPHA, GL_ONE); }
	void blendNormal() { glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }

	// ======================================================================== shapes

	void rect(float x, float y, float w, float h, const Rgb& color, float alpha) {
		setColor(color, alpha);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
	}

	void rectOutline(float x, float y, float w, float h, const Rgb& color, float alpha, float lineWidth) {
		glLineWidth(lineWidth);
		setColor(color, alpha);
		glBegin(GL_LINE_LOOP);
		glVertex2f(x + 0.5f, y + 0.5f);
		glVertex2f(x + w - 0.5f, y + 0.5f);
		glVertex2f(x + w - 0.5f, y + h - 0.5f);
		glVertex2f(x + 0.5f, y + h - 0.5f);
		glEnd();
	}

	void rectGradientV(float x, float y, float w, float h, const Rgb& top, float topAlpha, const Rgb& bottom, float bottomAlpha) {
		glBegin(GL_QUADS);
		setColor(top, topAlpha);
		glVertex2f(x, y);
		glVertex2f(x + w, y);
		setColor(bottom, bottomAlpha);
		glVertex2f(x + w, y + h);
		glVertex2f(x, y + h);
		glEnd();
	}

	void rectGradientH(float x, float y, float w, float h, const Rgb& left, float leftAlpha, const Rgb& right, float rightAlpha) {
		glBegin(GL_QUADS);
		setColor(left, leftAlpha);
		glVertex2f(x, y);
		setColor(right, rightAlpha);
		glVertex2f(x + w, y);
		glVertex2f(x + w, y + h);
		setColor(left, leftAlpha);
		glVertex2f(x, y + h);
		glEnd();
	}

	void line(float x1, float y1, float x2, float y2, const Rgb& color, float alpha, float lineWidth) {
		glLineWidth(lineWidth);
		setColor(color, alpha);
		glBegin(GL_LINES);
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
		glEnd();
	}

	void cornerBrackets(float x, float y, float w, float h, float armLength, const Rgb& color, float alpha, float lineWidth) {
		glLineWidth(lineWidth);
		setColor(color, alpha);

		const float l = armLength;
		glBegin(GL_LINES);
		// top-left
		glVertex2f(x, y);         glVertex2f(x + l, y);
		glVertex2f(x, y);         glVertex2f(x, y + l);
		// top-right
		glVertex2f(x + w, y);     glVertex2f(x + w - l, y);
		glVertex2f(x + w, y);     glVertex2f(x + w, y + l);
		// bottom-left
		glVertex2f(x, y + h);     glVertex2f(x + l, y + h);
		glVertex2f(x, y + h);     glVertex2f(x, y + h - l);
		// bottom-right
		glVertex2f(x + w, y + h); glVertex2f(x + w - l, y + h);
		glVertex2f(x + w, y + h); glVertex2f(x + w, y + h - l);
		glEnd();
	}

	void radialGlow(float cx, float cy, float radius, const Rgb& inner, float innerAlpha, const Rgb& outer, float outerAlpha, int segments) {
		glBegin(GL_TRIANGLE_FAN);
		setColor(inner, innerAlpha);
		glVertex2f(cx, cy);
		setColor(outer, outerAlpha);
		for (int i = 0; i <= segments; ++i) {
			const float a = 2.0f * EngineMath::PI * static_cast<float>(i) / segments;
			glVertex2f(cx + std::cos(a) * radius, cy + std::sin(a) * radius);
		}
		glEnd();
	}

	void ring(float cx, float cy, float radius, const Rgb& color, float alpha, float lineWidth, int segments) {
		glLineWidth(lineWidth);
		setColor(color, alpha);
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < segments; ++i) {
			const float a = 2.0f * EngineMath::PI * static_cast<float>(i) / segments;
			glVertex2f(cx + std::cos(a) * radius, cy + std::sin(a) * radius);
		}
		glEnd();
	}

	void chamferBox(float x, float y, float w, float h, float cut, const Rgb& color, float alpha) {
		setColor(color, alpha);
		// Convex hexagon, so a triangle fan from the first corner covers it exactly
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x, y);
		glVertex2f(x + w - cut, y);
		glVertex2f(x + w, y + cut);
		glVertex2f(x + w, y + h);
		glVertex2f(x + cut, y + h);
		glVertex2f(x, y + h - cut);
		glEnd();
	}

	void chamferOutline(float x, float y, float w, float h, float cut, const Rgb& color, float alpha, float lineWidth) {
		glLineWidth(lineWidth);
		setColor(color, alpha);
		glBegin(GL_LINE_LOOP);
		glVertex2f(x, y);
		glVertex2f(x + w - cut, y);
		glVertex2f(x + w, y + cut);
		glVertex2f(x + w, y + h);
		glVertex2f(x + cut, y + h);
		glVertex2f(x, y + h - cut);
		glEnd();
	}

	void progressBar(float x, float y, float w, float h, float fraction, const Rgb& fill, const Rgb& track, float alpha) {
		fraction = std::min(std::max(fraction, 0.0f), 1.0f);

		rect(x, y, w, h, track, alpha * 0.35f);
		rectGradientH(x, y, w * fraction, h, GamerTheme::scale(fill, 0.55f), alpha, fill, alpha);

		// the bright "leading edge" that makes it feel like it's actively charging
		if (fraction > 0.0f && fraction < 1.0f) {
			rect(x + w * fraction - 1.0f, y - 1.0f, 2.0f, h + 2.0f, GamerTheme::mixWhite(fill, 0.7f), alpha);
		}
	}

	void segmentBar(float x, float y, float w, float h, float fraction, int segments, const Rgb& fill, float alpha) {
		segments = std::max(1, segments);
		const float gap = 2.0f;
		const float segW = (w - gap * (segments - 1)) / segments;
		const float lit = fraction * segments;

		for (int i = 0; i < segments; ++i) {
			const float sx = x + i * (segW + gap);
			// fully lit, partially lit (the last one), or dim
			const float amount = std::min(std::max(lit - i, 0.0f), 1.0f);
			rect(sx, y, segW, h, fill, alpha * (0.15f + 0.85f * amount));
		}
	}

	// ======================================================================== bitmap text

	void text(float x, float y, const std::string& str, Font font, const Rgb& color, float alpha) {
		if (str.empty() || alpha <= 0.0f) {
			return;
		}

		// The raster color is latched when the raster position is set, so color goes FIRST
		setColor(color, alpha);
		rasterAt(x, y);

		void* glutFont = bitmapFont(font);
		for (char c : str) {
			glutBitmapCharacter(glutFont, static_cast<unsigned char>(c));
		}
	}

	int textWidth(const std::string& str, Font font) {
		return glutBitmapLength(bitmapFont(font), reinterpret_cast<const unsigned char*>(str.c_str()));
	}

	int fontHeight(Font font) {
		switch (font) {
			case Font::Tiny:      return 10;
			case Font::Body:      return 12;
			case Font::Large:     return 18;
			case Font::Mono:      return 15;
			case Font::MonoSmall: return 13;
		}
		return 12;
	}

	void textShadow(float x, float y, const std::string& str, Font font, const Rgb& color, float alpha) {
		text(x + 1.0f, y + 1.0f, str, font, Rgb{ 0.0f, 0.0f, 0.0f }, alpha * 0.75f);
		text(x, y, str, font, color, alpha);
	}

	void textGlow(float x, float y, const std::string& str, Font font, const Rgb& coreColor, const Rgb& glowColor, float alpha) {
		// Poor man's blur: stamp the text in the glow color at 8 offsets with additive
		// blending. The overlaps stack up brightest near the letters = soft neon halo.
		blendAdditive();
		static const float offsets[8][2] = {
			{ -1.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, -1.0f }, { 0.0f, 1.0f },
			{ -1.0f, -1.0f }, { 1.0f, 1.0f }, { -1.0f, 1.0f }, { 1.0f, -1.0f }
		};
		for (const auto& o : offsets) {
			text(x + o[0], y + o[1], str, font, glowColor, alpha * 0.22f);
		}
		blendNormal();
		text(x, y, str, font, coreColor, alpha);
	}

	// ======================================================================== stroke text

	float strokeTextWidth(const std::string& str, float capHeight, float letterSpacing) {
		if (str.empty()) {
			return 0.0f;
		}
		const float s = capHeight / strokeCapUnits;
		const float units = static_cast<float>(glutStrokeLength(GLUT_STROKE_ROMAN, reinterpret_cast<const unsigned char*>(str.c_str())));
		return units * s + letterSpacing * static_cast<float>(str.size() - 1);
	}

	float strokeText(float x, float y, const std::string& str, float capHeight, const Rgb& color, float alpha, float lineWidth, float letterSpacing) {
		if (str.empty() || alpha <= 0.0f) {
			return 0.0f;
		}

		const float s = capHeight / strokeCapUnits;

		glLineWidth(lineWidth);
		glEnable(GL_LINE_SMOOTH);
		setColor(color, alpha);

		glPushMatrix();
		glTranslatef(x, y, 0.0f);
		// Stroke fonts are drawn y-UP, our HUD space is y-DOWN, so flip y while scaling
		glScalef(s, -s, 1.0f);

		for (std::size_t i = 0; i < str.size(); ++i) {
			glutStrokeCharacter(GLUT_STROKE_ROMAN, static_cast<unsigned char>(str[i]));
			if (letterSpacing != 0.0f && i + 1 < str.size()) {
				glTranslatef(letterSpacing / s, 0.0f, 0.0f);
			}
		}

		glPopMatrix();
		glDisable(GL_LINE_SMOOTH);

		return strokeTextWidth(str, capHeight, letterSpacing);
	}

	void strokeTextNeon(float x, float y, const std::string& str, float capHeight, const Rgb& glowColor, float alpha, float letterSpacing) {
		// Four passes, fat to thin, like a real neon tube: a wide faint haze, a tighter
		// colored glow, a bright inner tube, and a near-white hot core. Additive blending
		// lets the passes pile up into that "this sign is humming" look.
		const float base = std::max(1.0f, capHeight / 14.0f);

		blendAdditive();
		strokeText(x, y, str, capHeight, glowColor, alpha * 0.10f, base * 3.2f, letterSpacing);
		strokeText(x, y, str, capHeight, glowColor, alpha * 0.22f, base * 2.0f, letterSpacing);
		strokeText(x, y, str, capHeight, glowColor, alpha * 0.65f, base * 1.2f, letterSpacing);
		strokeText(x, y, str, capHeight, GamerTheme::mixWhite(glowColor, 0.75f), alpha * 0.9f, std::max(1.0f, base * 0.6f), letterSpacing);
		blendNormal();
	}
}
