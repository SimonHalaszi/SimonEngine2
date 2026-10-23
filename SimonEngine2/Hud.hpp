#ifndef HUD_HPP
#define HUD_HPP

/*
 *   _  _ _   _ ___
 *  | || | | | |   \      SimonEngine2 :: CHUCK SPECIAL EDITION
 *  | __ | |_| | |) |     The Heads-Up Display. 40% real telemetry, 60% vibes.
 *  |_||_|\___/|___/
 *
 *  REAL numbers (measured or read straight from the engine):
 *      FPS, frame time, 1% lows, the frame-time graph, map name, object count,
 *      camera projection + zoom, theme, session timer, every toggle state
 *
 *  SMOKE AND MIRRORS (pure theater, animated with sine waves):
 *      "RTX 9090 Ti SUPER", GPU temp, CPU load, VRAM, PING 1ms, DLSS ULTRA,
 *      and the "FRAME GEN x4" line that just multiplies the real FPS by four
 *
 *  The HUD is stateless: ChuckEdition fills in a HudInfo every frame and Hud::draw()
 *  renders it. Panels slide in from the screen edges using the reveal value (0..1).
 */

#include <string>

#include "GamerTheme.hpp"

namespace Hud {

	struct HudInfo {
		float time = 0.0f;             // seconds since boot (drives the animations)
		float reveal = 1.0f;           // 0 = panels hidden off-screen, 1 = fully slid in

		// ---- performance (real)
		float fps = 0.0f;
		float frameMs = 0.0f;
		float onePercentLowFps = 0.0f;
		const float* frameHistoryMs = nullptr; // ring buffer of recent frame times
		int frameHistorySize = 0;
		int frameHistoryHead = 0;              // index of the OLDEST sample

		// ---- match (real)
		std::string mapName;
		std::string themeName;
		std::string cameraMode;
		float cameraZoom = 1.0f;
		int objectCount = 0;
		float sessionSeconds = 0.0f;

		// ---- toggles (real)
		bool rayTracing = true;
		bool showStats = true;
		bool isGameMap = false;   // Chuck World draws its own top-center panel + hints

		// ---- progression (real, earned with blood sweat and keypresses)
		int level = 1;
		std::string rank;
		int xp = 0;
		int xpLevelStart = 0;
		int xpLevelEnd = 100;

		// ---- esports scoreboard (fed by the totally-real killfeed)
		int chuckScore = 0;
		int bugsScore = 0;
	};

	void draw(const HudInfo& info, const GamerTheme::Theme& palette);

	// F1: the full controls sheet. alpha animates it in/out.
	void drawControls(const GamerTheme::Theme& palette, float time, float alpha);

	// The Konami reward: a rainbow confetti storm. age = seconds since it was triggered.
	void drawConfetti(float age);
}

#endif
