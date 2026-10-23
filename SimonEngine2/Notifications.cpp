#include "Notifications.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>

#include "Render2D.hpp"
#include "EngineMath.hpp"

namespace {
	using GamerTheme::Rgb;
	using Render2D::Font;

	const Rgb white = { 1.0f, 1.0f, 1.0f };
	const Rgb black = { 0.0f, 0.0f, 0.0f };
	const Rgb trophyGold = { 1.0f, 0.8f, 0.15f };

	// ---- timings (seconds)
	constexpr float toastSlide = 0.30f;
	constexpr float toastHold = 3.20f;
	constexpr float calloutLife = 1.60f;
	constexpr float calloutMinScreenTime = 0.90f;
	constexpr std::size_t calloutQueueMax = 4;
	constexpr float feedLife = 6.0f;
	constexpr std::size_t feedMax = 5;

	// -------------------------------------------------------------------------------
	//  THE TROPHY CASE
	//  Same order as the Achievement enum. XP values are scientifically balanced
	//  (they are not).
	// -------------------------------------------------------------------------------
	const AchievementInfo catalog[] = {
		{ "FIRST BLOOD",       "Pressed your very first key",              50 },
		{ "X-RAY VISION",      "Switched to wireframe",                    50 },
		{ "FILM NOIR",         "Went full grayscale",                      50 },
		{ "NOW YOU SEE ME",    "Made the robot vanish (F4)",               75 },
		{ "ELBOW GREASE",      "Bent an elbow",                           100 },
		{ "NOODLE ARMS",       "Bent the elbows 18 times. Please stop.",  250 },
		{ "TASTE THE RAINBOW", "Tried every single RGB theme",            200 },
		{ "CONSOLE PEASANT",   "Turned ray tracing OFF. Bold.",            25 },
		{ "WORLD TOURIST",     "Visited every map",                       250 },
		{ "POWER UP",          "Grabbed your first power core",           100 },
		{ "COMBO KING",        "Chained a x5 combo",                      300 },
		{ "VICTORY ROYALE",    "Secured all 10 cores",                    500 },
		{ "SPEEDRUNNER",       "Cleared a round in under 45 seconds",     750 },
		{ "PARKOUR!",          "Stood on top of the boss tower",          400 },
		{ "DIRECTOR'S CUT",    "Entered cinematic mode",                  100 },
		{ "CLIP IT!",          "Saved a screenshot",                      100 },
		{ "TOUCH GRASS",       "Played for 5 straight minutes",           300 },
		{ "CHUCK MODE",        "Up Up Down Down Left Right Left Right B A", 9001 },
	};

	static_assert(sizeof(catalog) / sizeof(catalog[0]) == static_cast<std::size_t>(Achievement::Count),
		"every Achievement needs a catalog entry");

	float clamp01(float v) {
		return std::min(std::max(v, 0.0f), 1.0f);
	}

	float easeOutBack(float t) {
		// Overshoots a little past 1 then settles: that satisfying "thunk" when a toast lands
		t = clamp01(t);
		const float c1 = 1.70158f;
		const float c3 = c1 + 1.0f;
		const float u = t - 1.0f;
		return 1.0f + c3 * u * u * u + c1 * u * u;
	}
}

const AchievementInfo& Notifications::info(Achievement achievement) {
	return catalog[static_cast<int>(achievement)];
}

void Notifications::achievement(Achievement which, float now) {
	toasts_.push_back(Toast{ which, -1.0f });
	update(now);
}

void Notifications::callout(const std::string& text, const std::string& subtitle, float now) {
	Callout next;
	next.text = text;
	next.subtitle = subtitle;
	next.born = now;
	if (!callouts_.empty()) {
		next.born = std::max(now, callouts_.back().born + calloutMinScreenTime);
	}
	callouts_.push_back(next);

	// Mashing keys shouldn't build a 30-second backlog: drop the oldest WAITING ones
	while (callouts_.size() > calloutQueueMax) {
		callouts_.erase(callouts_.begin() + 1);
	}
}

void Notifications::killfeed(const std::string& killer, const std::string& weapon, const std::string& victim, float now) {
	feed_.push_back(FeedEntry{ killer, weapon, victim, now });
	if (feed_.size() > feedMax) {
		feed_.erase(feed_.begin());
	}
}

void Notifications::update(float now) {
	// The front toast starts its clock when it reaches the front; when its time is up
	// it leaves and the next one in line starts
	if (!toasts_.empty() && toasts_.front().shownAt < 0.0f) {
		toasts_.front().shownAt = now;
	}
	while (!toasts_.empty() && now - toasts_.front().shownAt > toastSlide * 2.0f + toastHold) {
		toasts_.pop_front();
		if (!toasts_.empty()) {
			toasts_.front().shownAt = now;
		}
	}

	feed_.erase(std::remove_if(feed_.begin(), feed_.end(), [now](const FeedEntry& e) { return now - e.born > feedLife; }), feed_.end());

	// Retire the current callout once the next one's turn has come (or it simply expired)
	while (!callouts_.empty()) {
		const bool superseded = callouts_.size() >= 2 && callouts_[1].born <= now;
		const bool expired = now - callouts_.front().born > calloutLife;
		if (!superseded && !expired) {
			break;
		}
		callouts_.pop_front();
	}
}

void Notifications::draw(const GamerTheme::Theme& palette, float now, float feedTop) const {
	drawKillfeed(palette, now, feedTop);
	drawCallout(palette, now);
	drawAchievement(palette, now);
}

// ============================================================================ achievement toast

void Notifications::drawAchievement(const GamerTheme::Theme& p, float now) const {
	if (toasts_.empty() || toasts_.front().shownAt < 0.0f) {
		return;
	}

	const Toast& toast = toasts_.front();
	const AchievementInfo& a = info(toast.which);
	const float age = now - toast.shownAt;
	const float total = toastSlide * 2.0f + toastHold;

	// Slide up with a little overshoot, hold, then sink back down
	float shown = 1.0f;
	if (age < toastSlide) {
		shown = easeOutBack(age / toastSlide);
	}
	else if (age > total - toastSlide) {
		shown = 1.0f - clamp01((age - (total - toastSlide)) / toastSlide);
	}

	const float w = static_cast<float>(Render2D::width());
	const float h = static_cast<float>(Render2D::height());
	const float boxW = 320.0f;
	const float boxH = 58.0f;
	const float x = (w - boxW) * 0.5f;
	const float y = h - 80.0f - boxH * shown; // sits just above the XP bar / game hints
	const float alpha = clamp01(shown * 1.5f);

	// Glass pill + gold rim + a sheen that sweeps across once when it lands
	Render2D::chamferBox(x, y, boxW, boxH, 12.0f, GamerTheme::scale(p.skyTop, 0.5f), 0.85f * alpha);
	Render2D::chamferOutline(x, y, boxW, boxH, 12.0f, trophyGold, 0.9f * alpha, 1.5f);

	const float sheen = (age - toastSlide) / 0.6f;
	if (sheen > 0.0f && sheen < 1.0f) {
		const float sx = x + sheen * (boxW + 60.0f) - 60.0f;
		Render2D::blendAdditive();
		Render2D::rectGradientH(sx, y + 2.0f, 30.0f, boxH - 4.0f, white, 0.0f, white, 0.18f * alpha);
		Render2D::rectGradientH(sx + 30.0f, y + 2.0f, 30.0f, boxH - 4.0f, white, 0.18f * alpha, white, 0.0f);
		Render2D::blendNormal();
	}

	// Trophy medallion: a pulsing gold ring around a little cup made of rectangles
	const float cx = x + 30.0f;
	const float cy = y + boxH * 0.5f;
	Render2D::blendAdditive();
	Render2D::radialGlow(cx, cy, 26.0f, trophyGold, 0.35f * alpha, trophyGold, 0.0f, 32);
	Render2D::blendNormal();
	Render2D::ring(cx, cy, 18.0f, trophyGold, alpha, 2.0f, 40);
	Render2D::rect(cx - 7.0f, cy - 9.0f, 14.0f, 9.0f, trophyGold, alpha);   // cup
	Render2D::rect(cx - 1.5f, cy, 3.0f, 5.0f, trophyGold, alpha);           // stem
	Render2D::rect(cx - 6.0f, cy + 5.0f, 12.0f, 3.0f, trophyGold, alpha);   // base
	Render2D::rect(cx - 11.0f, cy - 8.0f, 4.0f, 2.0f, trophyGold, alpha);   // handles
	Render2D::rect(cx + 7.0f, cy - 8.0f, 4.0f, 2.0f, trophyGold, alpha);

	Render2D::text(x + 58.0f, y + 17.0f, "ACHIEVEMENT UNLOCKED", Font::Tiny, trophyGold, alpha);
	Render2D::textShadow(x + 58.0f, y + 36.0f, a.title, Font::Large, white, alpha);
	Render2D::text(x + 58.0f, y + 50.0f, a.description, Font::Tiny, white, 0.65f * alpha);

	const std::string xp = "+" + std::to_string(a.xp) + " XP";
	const float xw = static_cast<float>(Render2D::textWidth(xp, Font::Body));
	Render2D::textGlow(x + boxW - 14.0f - xw, y + 36.0f, xp, Font::Body, white, p.accent, alpha);
}

// ============================================================================ callout

void Notifications::drawCallout(const GamerTheme::Theme& p, float now) const {
	if (callouts_.empty()) {
		return;
	}
	const Callout& current = callouts_.front();
	const float age = now - current.born;
	if (age < 0.0f || age > calloutLife || current.text.empty()) {
		return;
	}

	const float w = static_cast<float>(Render2D::width());
	const float h = static_cast<float>(Render2D::height());

	// Punch in (starts big, slams to size), hold, fade out
	const float punch = clamp01(age / 0.14f);
	const float fade = clamp01((calloutLife - age) / 0.4f);
	const float alpha = punch * fade;
	const float cap = 26.0f + (1.0f - punch) * 18.0f;
	const float spacing = 4.0f;

	const float tw = Render2D::strokeTextWidth(current.text, cap, spacing);
	const float y = h * 0.27f;

	// A dark band behind it so it reads on any background
	Render2D::rectGradientH(0.0f, y - cap - 14.0f, w * 0.5f, cap + 40.0f, black, 0.0f, black, 0.45f * alpha);
	Render2D::rectGradientH(w * 0.5f, y - cap - 14.0f, w * 0.5f, cap + 40.0f, black, 0.45f * alpha, black, 0.0f);

	// Two quick "glitch" offset copies in the theme colors during the punch-in
	if (age < 0.25f) {
		const float jitter = (0.25f - age) * 40.0f;
		Render2D::blendAdditive();
		Render2D::strokeText((w - tw) * 0.5f - jitter, y, current.text, cap, p.accent2, 0.5f * alpha, 2.0f, spacing);
		Render2D::strokeText((w - tw) * 0.5f + jitter, y, current.text, cap, p.accent, 0.5f * alpha, 2.0f, spacing);
		Render2D::blendNormal();
	}

	Render2D::strokeTextNeon((w - tw) * 0.5f, y, current.text, cap, p.accent, alpha, spacing);

	if (!current.subtitle.empty()) {
		const float sw = static_cast<float>(Render2D::textWidth(current.subtitle, Font::Body));
		Render2D::textShadow((w - sw) * 0.5f, y + 20.0f, current.subtitle, Font::Body, p.accent2, alpha);
	}
}

// ============================================================================ killfeed

void Notifications::drawKillfeed(const GamerTheme::Theme& p, float now, float feedTop) const {
	const float w = static_cast<float>(Render2D::width());
	float y = feedTop;

	// Newest at the top
	for (auto it = feed_.rbegin(); it != feed_.rend(); ++it) {
		const FeedEntry& e = *it;
		const float age = now - e.born;
		const float slide = (1.0f - clamp01(age / 0.2f)) * 60.0f;
		const float alpha = clamp01((feedLife - age) / 0.6f) * clamp01(age / 0.15f);

		const std::string weapon = "[" + e.weapon + "]";
		const float kw = static_cast<float>(Render2D::textWidth(e.killer, Font::Body));
		const float ww = static_cast<float>(Render2D::textWidth(weapon, Font::Tiny));
		const float vw = static_cast<float>(Render2D::textWidth(e.victim, Font::Body));
		const float rowW = kw + ww + vw + 28.0f;
		const float x = w - 14.0f - rowW + slide;

		// The newest kill gets a brighter background for a moment, like the real thing
		const float fresh = 1.0f - clamp01(age / 1.0f);
		Render2D::rect(x, y, rowW, 19.0f, black, (0.45f + 0.2f * fresh) * alpha);
		Render2D::rect(x, y, 2.0f, 19.0f, p.accent, alpha);

		float cx = x + 8.0f;
		Render2D::text(cx, y + 14.0f, e.killer, Font::Body, p.accent, alpha);
		cx += kw + 6.0f;
		Render2D::text(cx, y + 13.0f, weapon, Font::Tiny, white, 0.85f * alpha);
		cx += ww + 6.0f;
		Render2D::text(cx, y + 14.0f, e.victim, Font::Body, p.accent2, alpha);

		y += 22.0f;
	}
}
