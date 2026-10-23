#ifndef STUDIO_LIGHTING_HPP
#define STUDIO_LIGHTING_HPP

/*
 *   ___ _______  __   ___  _  _
 *  | _ \_   _\ \/ /  / _ \| \| |     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  |   / | |  >  <  | (_) | .` |     "Ray Tracing" (tm) (not actual ray tracing)
 *  |_|_\ |_| /_/\_\  \___/|_|\_|
 *
 *  Full disclosure for anyone reading the source: the HUD calls this RAY TRACING, but it
 *  is a classic three-point studio light setup using plain fixed-function OpenGL lights,
 *  shaded per vertex (Gouraud). Your buddy does not need to know that. Your buddy only
 *  needs to know that the robot went from a flat blue sticker to a shaded 3D model with
 *  colored rim lights the moment F7 got pressed.
 *
 *  The rig (all directional lights, all defined in CAMERA space so they orbit with you):
 *
 *        KEY  (warm white)    upper-left, in front of the subject: the main light
 *        RIM  (theme accent)  hard from the left and a bit behind: colored edge glow
 *        KICK (theme accent2) from the right and a bit behind: the second neon edge
 *
 *  Because the lights ride along with the camera, the "cyan on the left, magenta on the
 *  right" look holds no matter where you orbit. Every esports thumbnail ever, basically.
 */

#include "GamerTheme.hpp"

namespace StudioLighting {

	// Turns the rig on, configured with this frame's palette. Call with the camera's
	// projection already set; the modelview matrix is saved and restored internally.
	void apply(const GamerTheme::Theme& palette);

	// Turns all three lights (and lighting itself) back off
	void disable();
}

#endif
