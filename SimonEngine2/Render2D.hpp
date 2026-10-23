#ifndef RENDER_2D_HPP
#define RENDER_2D_HPP

/*
 *   ___ ___ _  _ ___  ___ ___   ___ ___
 *  | _ \ __| \| |   \| __| _ \ |_  )   \     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  |   / _|| .` | |) | _||   /  / /| |) |    Immediate-mode 2D toolkit for HUDs,
 *  |_|_\___|_|\_|___/|___|_|_\ /___|___/     splash screens and assorted eye candy
 *
 *  Everything here draws in SCREEN PIXELS with (0,0) at the TOP-LEFT and y going DOWN,
 *  because that is how every HUD designer on earth thinks. Wrap your drawing in
 *  Render2D::begin() / Render2D::end() and the toolkit handles the projection swap,
 *  saves and restores every bit of GL state, and makes sure the 3D lighting, depth test
 *  and neon texture tricks don't leak into the overlay (or the other way around).
 *
 *  Text comes in two flavors:
 *    - bitmap text: crisp, fixed size, great for stats and small labels
 *    - stroke text: vector lines, any size, can be made thick and glowy (titles, logos)
 */

#include <string>

#include "GamerTheme.hpp"

namespace Render2D {

	using GamerTheme::Rgb;

	enum class Font {
		Tiny,      // Helvetica 10
		Body,      // Helvetica 12
		Large,     // Helvetica 18
		Mono,      // 9x15 fixed: the "terminal hacker" font for numbers
		MonoSmall  // 8x13 fixed
	};

	// ------------------------------------------------------------------ frame setup

	// Enter 2D mode: pushes all GL state + both matrices, sets a pixel-perfect ortho projection
	void begin();
	// Leave 2D mode: restores everything begin() touched
	void end();

	int width();   // window size in pixels, as of the last begin()
	int height();

	// Additive blending makes overlapping colors ADD up toward white = instant glow.
	// Normal blending is regular alpha transparency. begin() starts in normal mode.
	void blendAdditive();
	void blendNormal();

	// ------------------------------------------------------------------ shapes

	void rect(float x, float y, float w, float h, const Rgb& color, float alpha);
	void rectOutline(float x, float y, float w, float h, const Rgb& color, float alpha, float lineWidth = 1.0f);

	// Vertical gradient (top color -> bottom color)
	void rectGradientV(float x, float y, float w, float h, const Rgb& top, float topAlpha, const Rgb& bottom, float bottomAlpha);
	// Horizontal gradient (left color -> right color)
	void rectGradientH(float x, float y, float w, float h, const Rgb& left, float leftAlpha, const Rgb& right, float rightAlpha);

	void line(float x1, float y1, float x2, float y2, const Rgb& color, float alpha, float lineWidth = 1.0f);

	// Those little L-shaped sci-fi corner marks around a box
	void cornerBrackets(float x, float y, float w, float h, float armLength, const Rgb& color, float alpha, float lineWidth = 1.5f);

	// Filled circle with a different color/alpha at the center vs. the rim (radial gradient)
	void radialGlow(float cx, float cy, float radius, const Rgb& inner, float innerAlpha, const Rgb& outer, float outerAlpha, int segments = 48);

	void ring(float cx, float cy, float radius, const Rgb& color, float alpha, float lineWidth = 1.5f, int segments = 64);

	// A filled box with the top-right and bottom-left corners chamfered off (very "gamer UI")
	void chamferBox(float x, float y, float w, float h, float cut, const Rgb& color, float alpha);
	void chamferOutline(float x, float y, float w, float h, float cut, const Rgb& color, float alpha, float lineWidth = 1.0f);

	// A horizontal progress bar with a background track, fill, and a bright leading edge
	void progressBar(float x, float y, float w, float h, float fraction, const Rgb& fill, const Rgb& track, float alpha);

	// A bar chopped into little blocks, like a health bar in every fighting game ever
	void segmentBar(float x, float y, float w, float h, float fraction, int segments, const Rgb& fill, float alpha);

	// ------------------------------------------------------------------ bitmap text

	// Draws text with its BASELINE at y. Safe to call with x/y partly off-screen
	// (plain glRasterPos would silently drop the whole string; see Render2D.cpp).
	void text(float x, float y, const std::string& str, Font font, const Rgb& color, float alpha);
	int textWidth(const std::string& str, Font font);
	int fontHeight(Font font);

	// Text with a 1px dark drop shadow so it stays readable on any background
	void textShadow(float x, float y, const std::string& str, Font font, const Rgb& color, float alpha);
	// Text with a soft neon halo (glowColor) behind a bright core (coreColor)
	void textGlow(float x, float y, const std::string& str, Font font, const Rgb& coreColor, const Rgb& glowColor, float alpha);

	// ------------------------------------------------------------------ stroke text

	// Vector text. x/y = left end of the baseline, capHeight = height of a capital letter in pixels.
	// letterSpacing adds extra pixels between characters (for that W I D E look).
	// Returns the width drawn.
	float strokeText(float x, float y, const std::string& str, float capHeight, const Rgb& color, float alpha, float lineWidth = 1.5f, float letterSpacing = 0.0f);
	float strokeTextWidth(const std::string& str, float capHeight, float letterSpacing = 0.0f);

	// Neon sign version: a fat faint glow pass, a medium pass, then a thin hot-white core
	void strokeTextNeon(float x, float y, const std::string& str, float capHeight, const Rgb& glowColor, float alpha, float letterSpacing = 0.0f);
}

#endif
