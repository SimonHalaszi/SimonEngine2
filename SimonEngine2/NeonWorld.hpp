#ifndef NEON_WORLD_HPP
#define NEON_WORLD_HPP

/*
 *   _  _ ___ ___  _  _  __      _____  ___ _    ___
 *  | \| | __/ _ \| \| | \ \    / / _ \| _ \ |  |   \     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  | .` | _| (_) | .` |  \ \/\/ / (_) |   / |__| |) |    World-space neon: the Tron floor
 *  |_|\_|___\___/|_|\_|   \_/\_/ \___/|_|_\____|___/     and the glowing edge outlines
 *
 *  Unlike the 2D backdrop, these effects live INSIDE the 3D scene: they use the scene's
 *  camera and the depth buffer, so the robot properly stands on the floor and the
 *  outlines hug the actual geometry.
 *
 *  FLOOR    A scrolling neon grid that fades out with distance, a soft contact shadow
 *           under the model (so it looks grounded instead of floating), and a pulsing
 *           "spawn pad" ring with a rotating dashed outer ring.
 *
 *  OUTLINES The whole scene gets drawn a second (and third) time with polygons in LINE
 *           mode and every color forced to the theme accent. A fat faint pass + a thin
 *           bright pass = glowing neon edges. The color override is the sneakiest trick
 *           in the codebase; the gory details are in NeonWorld.cpp.
 */

#include "GamerTheme.hpp"

namespace NeonWorld {

	// Draws the floor at height floorY. padRadius ~ how wide the object standing on it is.
	// Call AFTER the scene's opaque objects so the depth buffer can hide the floor where
	// objects cover it.
	// extent = radius where the grid fades out, spacing = distance between grid lines
	void drawFloor(const GamerTheme::Theme& palette, float timeSeconds, float floorY, float padRadius, float extent = 8.0f, float spacing = 0.5f);

	// How many times the scene must be redrawn for the outline effect
	int outlinePassCount();

	// Wrap ONE redraw of the scene's objects between these two calls per pass
	void beginOutlinePass(const GamerTheme::Theme& palette, int pass);
	void endOutlinePass();
}

#endif
