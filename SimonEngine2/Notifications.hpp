#ifndef NOTIFICATIONS_HPP
#define NOTIFICATIONS_HPP

/*
 *   _  _  ___ _____ ___ ___ ___
 *  | \| |/ _ \_   _|_ _| __/ __|     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  | .` | (_) || |  | || _|\__ \     Dopamine Delivery Subsystem
 *  |_|\_|\___/ |_| |___|_| |___/
 *
 *  Three flavors of "something happened!!":
 *
 *    ACHIEVEMENTS  Xbox-style toast that slides up from the bottom with a chime-shaped
 *                  hole where the sound effect would go. Queued, shown one at a time.
 *    CALLOUTS      Big neon text punched into the middle of the screen when you flip a
 *                  mode ("WIREFRAME", "RAY TRACING: OFF"...). Newest replaces oldest.
 *    KILLFEED      Top-right esports feed. "Chuck [HEADSHOT] Segfault". Mostly lies.
 *
 *  Plus the achievement catalog itself. Unlock bookkeeping lives in ChuckEdition; this
 *  module just knows what each achievement is called and how to draw things.
 */

#include <deque>
#include <string>
#include <vector>

#include "GamerTheme.hpp"

enum class Achievement {
	FirstBlood,
	XRayVision,
	FilmNoir,
	NowYouSeeMe,
	ElbowGrease,
	NoodleArms,
	TasteTheRainbow,
	ConsolePeasant,
	WorldTourist,
	PowerUp,
	ComboKing,
	VictoryRoyale,
	Speedrunner,
	Parkour,
	Directors,
	Shutterbug,
	TouchGrass,
	Konami,
	Count // keep last
};

struct AchievementInfo {
	const char* title;
	const char* description;
	int xp;
};

class Notifications {
	public:
		static const AchievementInfo& info(Achievement achievement);

		void achievement(Achievement which, float now);
		void callout(const std::string& text, const std::string& subtitle, float now);
		void killfeed(const std::string& killer, const std::string& weapon, const std::string& victim, float now);

		// Callouts queue up: each one gets at least a short moment on screen before the next
		// bumps it, so "WIREFRAME" isn't instantly stomped by the "LEVEL UP" it caused.
		// Draws everything that's currently alive. Call inside Render2D::begin()/end().
		// feedTop = y where the killfeed starts (just under the performance panel)
		void draw(const GamerTheme::Theme& palette, float now, float feedTop) const;

		// Drops expired entries and advances the achievement queue
		void update(float now);

	private:
		struct Toast {
			Achievement which;
			float shownAt;   // -1 until it reaches the front of the queue
		};

		struct Callout {
			std::string text;
			std::string subtitle;
			float born = -100.0f;
		};

		struct FeedEntry {
			std::string killer;
			std::string weapon;
			std::string victim;
			float born;
		};

		void drawAchievement(const GamerTheme::Theme& p, float now) const;
		void drawCallout(const GamerTheme::Theme& p, float now) const;
		void drawKillfeed(const GamerTheme::Theme& p, float now, float feedTop) const;

		std::deque<Toast> toasts_;
		std::deque<Callout> callouts_; // front = on screen now, the rest are waiting their turn
		std::vector<FeedEntry> feed_;
};

#endif
