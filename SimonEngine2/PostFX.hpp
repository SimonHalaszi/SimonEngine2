#ifndef POST_FX_HPP
#define POST_FX_HPP

/*
 *   ___  ___  ___ _____   _____  __
 *  | _ \/ _ \/ __|_   _| | __\ \/ /     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  |  _/ (_) \__ \ | |   | _| >  <      "Post processing" (there is no framebuffer
 *  |_|  \___/|___/ |_|   |_| /_/\_\     post processing, it's all translucent 2D on top)
 *
 *  Real post-processing would render the scene to a texture and run it through shaders.
 *  This engine is OpenGL 1.1 immediate mode with zero shaders, so instead every "post
 *  effect" here is a translucent 2D layer drawn over the finished frame. It's smoke and
 *  mirrors, and it looks great:
 *
 *    vignette   dark gradients creeping in from the edges (always on, subtle)
 *    letterbox  cinematic black bars that slide in for F5
 *    crt        F8: scanlines, an RGB aperture grille, a rolling bright band,
 *               flicker, and the occasional glitch tear. 1998 called.
 *    flash      the white camera flash when F9 saves a screenshot
 *
 *  Everything must be called inside Render2D::begin()/end().
 */

#include "GamerTheme.hpp"

namespace PostFX {

	void vignette(float strength);
	void letterbox(float amount);  // 0 = hidden, 1 = fully in
	void crt(const GamerTheme::Theme& palette, float timeSeconds);
	void flash(float alpha);
}

#endif
