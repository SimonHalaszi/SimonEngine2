#ifndef GAMER_THEME_HPP
#define GAMER_THEME_HPP

/*
 *   ___  ___ ___
 *  | _ \/ __| _ )     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  |   / (_ | _ \     RGB Theme System(tm)
 *  |_|_\\___|___/     "every pro knows RGB adds at least +15 FPS"
 *
 *  A Theme is a hand-tuned palette that every Chuck Edition effect pulls its colors from:
 *  the sky gradient, the synthwave sun, the neon floor grid, the studio rim lights, the
 *  HUD accents, the outline glow... all of it. Swap the theme (F2) and the whole engine
 *  re-skins itself with a smooth crossfade instead of a hard cut.
 *
 *  Rainbow themes ("RGB OVERDRIVE", and the secret one you unlock with the Konami code)
 *  ignore their fixed accents and spin the hue wheel over time instead.
 */

#include <string>

namespace GamerTheme {

	// Tiny float color. Deliberately NOT called "RGB": <windows.h> (pulled in by freeglut)
	// defines an RGB(r,g,b) macro that would eat this struct alive on MSVC.
	struct Rgb {
		float r, g, b;
	};

	struct Theme {
		const char* name;    // shown on the HUD and in the theme-switch callout
		Rgb accent;          // primary neon (HUD text glow, outlines, left rim light)
		Rgb accent2;         // secondary neon (killfeed victims, right rim light)
		Rgb skyTop;          // backdrop gradient, top of the screen
		Rgb skyHorizon;      // backdrop gradient, where the sun sits
		Rgb sunTop;          // synthwave sun, top edge
		Rgb sunBottom;       // synthwave sun, bottom edge
		Rgb grid;            // neon floor grid lines
		bool rainbow;        // true = accents cycle through the hue wheel
	};

	// Number of themes you can cycle through with F2 (the secret theme is not counted)
	int themeCount();

	// Theme by index (wraps around, so any int is safe)
	const Theme& theme(int index);

	// The hidden theme unlocked by the Konami code. Shh.
	const Theme& chuckModeTheme();

	// ---------------------------------------------------------------- color math helpers

	Rgb lerp(const Rgb& a, const Rgb& b, float t);
	Rgb scale(const Rgb& c, float s);
	Rgb mixWhite(const Rgb& c, float amount); // amount 0 = original, 1 = pure white

	// HSV -> RGB. hue is in turns (0..1 wraps), saturation/value are 0..1
	Rgb hsv(float hue, float saturation, float value);

	// Blends two whole themes (used for the smooth crossfade when switching)
	Theme blend(const Theme& from, const Theme& to, float t);

	// Applies rainbow hue cycling (if the theme asks for it) at time t seconds.
	// Returns the palette the renderer should actually use this frame.
	Theme resolve(const Theme& base, float timeSeconds);
}

#endif
