#ifndef SPLASH_SCREEN_HPP
#define SPLASH_SCREEN_HPP

/*
 *   ___ ___ _      _   ___ _  _
 *  / __| _ \ |    /_\ / __| || |     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  \__ \  _/ |__ / _ \\__ \ __ |     The boot screen. Builds anticipation. Loads nothing.
 *  |___/_| |____/_/ \_\___/_||_|
 *
 *  Timeline (seconds since boot):
 *     0.0 - 0.7   the logo flickers on like a neon sign warming up
 *     0.3 - 3.4   fake boot log lines tick in one by one, progress bar fills
 *                 (it stalls at 99%, because of course it does)
 *     3.6+        PRESS ANY KEY blinks
 *     4.6         auto-continues if nobody pressed anything
 *     then        0.6s fade out revealing the game, HUD slides in
 *
 *  Any key or click skips it. ChuckEdition swallows that input so it doesn't ALSO
 *  toggle wireframe or whatever the key normally does.
 */

#include "GamerTheme.hpp"

namespace SplashScreen {

	constexpr float autoContinueSeconds = 4.6f;
	constexpr float fadeSeconds = 0.6f;

	// t = seconds since boot, alpha = overall opacity (fades to 0 when leaving)
	void draw(const GamerTheme::Theme& palette, float t, float alpha);
}

#endif
