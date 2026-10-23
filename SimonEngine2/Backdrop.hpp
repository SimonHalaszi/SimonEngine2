#ifndef BACKDROP_HPP
#define BACKDROP_HPP

/*
 *   ___   _   ___ _  _____  ___  ___  ___
 *  | _ ) /_\ / __| |/ /   \| _ \/ _ \| _ \     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  | _ \/ _ \ (__| ' <| |) |   / (_) |  _/     The "it's 1986 and we're driving a
 *  |___/_/ \_\___|_|\_\___/|_|_\\___/|_|       Testarossa into the sunset" backdrop
 *
 *  Painted in 2D straight onto the freshly cleared screen BEFORE the 3D scene, with depth
 *  testing off, so it can never cover the actual game objects. Layers, back to front:
 *
 *      1. sky gradient        (theme skyTop -> skyHorizon)
 *      2. stars               (twinkling, fade out toward the horizon)
 *      3. sun glow + sun      (striped synthwave sun, stripes slowly scroll)
 *      4. city skyline        (procedural silhouettes with lit windows)
 *      5. ground haze         (dark gradient below the horizon)
 *      6. embers              (little glowing particles drifting upward)
 *
 *  Everything is generated from a deterministic hash + the clock, so there's no state,
 *  no allocation and nothing to update: the whole sky is a pure function of time.
 */

#include "GamerTheme.hpp"

namespace Backdrop {

	// Paints the full-screen backdrop. Call right after glClear.
	// horizonFraction = where the horizon sits, 0 = top of the screen, 1 = bottom.
	// (ChuckEdition lines this up with the 3D camera's real horizon in perspective scenes.)
	void draw(const GamerTheme::Theme& palette, float timeSeconds, float horizonFraction = 0.60f);

	// A few big, soft embers drawn IN FRONT of the 3D scene (call from the overlay pass)
	// for a cheap depth-of-field / parallax feel
	void drawForegroundEmbers(const GamerTheme::Theme& palette, float timeSeconds);
}

#endif
