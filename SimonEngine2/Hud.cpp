#include "Hud.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "Render2D.hpp"

namespace Hud {

	using GamerTheme::Rgb;
	using Render2D::Font;

	namespace {
		constexpr float margin = 14.0f;
		const Rgb white = { 1.0f, 1.0f, 1.0f };
		const Rgb black = { 0.0f, 0.0f, 0.0f };

		// Ease-out cubic: fast start, gentle landing. Every slide-in animation uses this.
		float easeOut(float t) {
			t = std::min(std::max(t, 0.0f), 1.0f);
			const float inv = 1.0f - t;
			return 1.0f - inv * inv * inv;
		}

		std::string fixed(float value, int decimals) {
			std::ostringstream out;
			out << std::fixed << std::setprecision(decimals) << value;
			return out.str();
		}

		std::string clock(float seconds) {
			const int total = static_cast<int>(seconds);
			std::ostringstream out;
			out << std::setfill('0') << std::setw(2) << total / 3600 << ":"
				<< std::setw(2) << (total / 60) % 60 << ":" << std::setw(2) << total % 60;
			return out.str();
		}

		// Spreads a string out letter by letter: "CHUCK" -> "C H U C K".
		// Bitmap fonts can't letter-space, so we cheat with literal spaces.
		std::string spaced(const std::string& s) {
			std::string out;
			for (std::size_t i = 0; i < s.size(); ++i) {
				out += s[i];
				if (i + 1 < s.size()) {
					out += ' ';
				}
			}
			return out;
		}

		// The standard Chuck Edition panel: dark glass, chamfered corners, neon rim,
		// and a little title tab sticking out of the top-left
		void panel(float x, float y, float w, float h, const std::string& title, const GamerTheme::Theme& p, float alpha) {
			Render2D::chamferBox(x, y, w, h, 10.0f, GamerTheme::scale(p.skyTop, 0.6f), 0.62f * alpha);
			Render2D::chamferOutline(x, y, w, h, 10.0f, p.accent, 0.55f * alpha);

			if (!title.empty()) {
				const float tabW = static_cast<float>(Render2D::textWidth(title, Font::Tiny)) + 16.0f;
				Render2D::chamferBox(x, y - 12.0f, tabW, 13.0f, 5.0f, p.accent, 0.85f * alpha);
				Render2D::text(x + 8.0f, y - 2.0f, title, Font::Tiny, black, alpha);
			}
		}

		// One "LABEL ........ value" line in a stats panel
		void statRow(float x, float y, float w, const std::string& label, const std::string& value, const Rgb& valueColor, float alpha) {
			Render2D::text(x, y, label, Font::MonoSmall, white, 0.55f * alpha);
			const float vw = static_cast<float>(Render2D::textWidth(value, Font::MonoSmall));
			Render2D::text(x + w - vw, y, value, Font::MonoSmall, valueColor, alpha);
		}

		// ------------------------------------------------------------------ top-left
		void drawLogo(const HudInfo& info, const GamerTheme::Theme& p) {
			const float slide = (1.0f - easeOut(info.reveal)) * -260.0f;
			const float x = margin + slide;

			// The big glowing logo, with a slow "neon flicker" every few seconds
			const float flicker = (std::fmod(info.time, 7.3f) < 0.12f) ? 0.45f : 1.0f;
			Render2D::strokeTextNeon(x + 2.0f, 34.0f, "SIMONENGINE2", 18.0f, p.accent, flicker, 1.5f);

			Render2D::text(x + 3.0f, 50.0f, spaced("CHUCK SPECIAL EDITION"), Font::Tiny, p.accent2, 0.95f);

			// version pill
			const std::string version = "v2.0.69-PRO";
			const float vx = x + 3.0f + Render2D::textWidth(spaced("CHUCK SPECIAL EDITION"), Font::Tiny) + 8.0f;
			const float vw = static_cast<float>(Render2D::textWidth(version, Font::Tiny)) + 10.0f;
			Render2D::chamferBox(vx, 40.0f, vw, 13.0f, 4.0f, p.accent2, 0.85f);
			Render2D::text(vx + 5.0f, 50.0f, version, Font::Tiny, black, 1.0f);
		}

		// ------------------------------------------------------------------ top-right
		void drawPerformance(const HudInfo& info, const GamerTheme::Theme& p, float screenW) {
			const float boxW = 176.0f;
			const float slide = (1.0f - easeOut(info.reveal)) * 260.0f;
			const float x = screenW - margin - boxW + slide;
			const float y = 22.0f;

			panel(x, y, boxW, 104.0f, "PERFORMANCE", p, 1.0f);

			// Big FPS number. Green if smooth, yellow if meh, red if Simon's laptop.
			const Rgb good = { 0.35f, 1.0f, 0.45f };
			const Rgb meh = { 1.0f, 0.85f, 0.2f };
			const Rgb bad = { 1.0f, 0.25f, 0.25f };
			const Rgb fpsColor = info.fps >= 100.0f ? good : (info.fps >= 45.0f ? meh : bad);

			const std::string fpsText = std::to_string(static_cast<int>(std::lround(info.fps)));
			Render2D::strokeTextNeon(x + 12.0f, y + 34.0f, fpsText, 22.0f, fpsColor, 1.0f, 1.0f);
			const float numW = Render2D::strokeTextWidth(fpsText, 22.0f, 1.0f);
			Render2D::text(x + 18.0f + numW, y + 34.0f, "FPS", Font::Body, fpsColor, 0.9f);

			Render2D::text(x + boxW - 66.0f, y + 20.0f, fixed(info.frameMs, 2) + " ms", Font::MonoSmall, white, 0.8f);
			Render2D::text(x + boxW - 66.0f, y + 34.0f, "1%L " + std::to_string(static_cast<int>(info.onePercentLowFps)), Font::MonoSmall, white, 0.55f);

			// ---- frame-time graph: the last N frames as a filled area, newest on the right
			const float gx = x + 10.0f;
			const float gy = y + 44.0f;
			const float gw = boxW - 20.0f;
			const float gh = 34.0f;
			Render2D::rect(gx, gy, gw, gh, black, 0.35f);

			if (info.frameHistoryMs && info.frameHistorySize > 1) {
				float worst = 1.0f;
				for (int i = 0; i < info.frameHistorySize; ++i) {
					worst = std::max(worst, info.frameHistoryMs[i]);
				}
				const float scaleMax = std::max(worst * 1.2f, 20.0f);

				// 60 FPS reference line (16.7 ms) so spikes have something to be judged against
				const float refY = gy + gh - (16.7f / scaleMax) * gh;
				Render2D::line(gx, refY, gx + gw, refY, p.accent2, 0.35f, 1.0f);
				Render2D::text(gx + 2.0f, refY - 2.0f, "16.7", Font::Tiny, p.accent2, 0.5f);

				glBegin(GL_QUAD_STRIP);
				for (int i = 0; i < info.frameHistorySize; ++i) {
					const float ms = info.frameHistoryMs[(info.frameHistoryHead + i) % info.frameHistorySize];
					const float px = gx + gw * i / (info.frameHistorySize - 1);
					const float py = gy + gh - std::min(ms / scaleMax, 1.0f) * gh;
					glColor4f(p.accent.r, p.accent.g, p.accent.b, 0.05f);
					glVertex2f(px, gy + gh);
					glColor4f(p.accent.r, p.accent.g, p.accent.b, 0.45f);
					glVertex2f(px, py);
				}
				glEnd();

				glLineWidth(1.0f);
				glColor4f(p.accent.r, p.accent.g, p.accent.b, 0.95f);
				glBegin(GL_LINE_STRIP);
				for (int i = 0; i < info.frameHistorySize; ++i) {
					const float ms = info.frameHistoryMs[(info.frameHistoryHead + i) % info.frameHistorySize];
					glVertex2f(gx + gw * i / (info.frameHistorySize - 1), gy + gh - std::min(ms / scaleMax, 1.0f) * gh);
				}
				glEnd();
			}

			// The single most important smoke-and-mirrors line in the whole engine
			const int frameGen = static_cast<int>(std::lround(info.fps * 4.0f));
			Render2D::text(x + 10.0f, y + 94.0f, "FRAME GEN x4 >> " + std::to_string(frameGen) + " FPS", Font::Tiny, p.accent2, 0.9f);
		}

		// ------------------------------------------------------------------ left side
		void drawStats(const HudInfo& info, const GamerTheme::Theme& p) {
			const float boxW = 204.0f;
			const float slide = (1.0f - easeOut(info.reveal * 1.2f - 0.2f)) * -300.0f;
			const float x = margin + slide;
			float y = 78.0f;
			const float rowH = 15.0f;
			const float innerX = x + 10.0f;
			const float innerW = boxW - 20.0f;
			const float t = info.time;

			// -------- SYSTEM: every number in here is fake. Every. Single. One.
			panel(x, y, boxW, 8.0f + rowH * 7.0f, "SYSTEM", p, 1.0f);
			y += 18.0f;

			const float gpuTemp = 41.0f + 3.0f * std::sin(t * 0.31f) + 1.2f * std::sin(t * 1.7f);
			const float cpuLoad = 9.0f + 5.0f * std::fabs(std::sin(t * 0.53f)) + 2.0f * std::sin(t * 3.1f);
			const float vram = 13.37f + 0.4f * std::sin(t * 0.2f);

			statRow(innerX, y, innerW, "GPU", "RTX 9090 Ti SUPER", p.accent, 1.0f);
			y += rowH;
			statRow(innerX, y, innerW, "VRAM", fixed(vram, 2) + " / 420 GB", white, 0.9f);
			y += rowH;
			statRow(innerX, y, innerW, "TEMP", std::to_string(static_cast<int>(gpuTemp)) + " C", white, 0.9f);
			Render2D::segmentBar(innerX + 52.0f, y - 8.0f, 70.0f, 7.0f, gpuTemp / 90.0f, 8, p.accent, 0.9f);
			y += rowH;
			statRow(innerX, y, innerW, "CPU", std::to_string(static_cast<int>(cpuLoad)) + " %", white, 0.9f);
			Render2D::segmentBar(innerX + 52.0f, y - 8.0f, 70.0f, 7.0f, cpuLoad / 100.0f, 8, p.accent2, 0.9f);
			y += rowH;
			statRow(innerX, y, innerW, "PING", "1 ms (LAN)", white, 0.9f);
			y += rowH;
			const Rgb on = { 0.35f, 1.0f, 0.45f };
			const Rgb off = { 1.0f, 0.3f, 0.3f };
			statRow(innerX, y, innerW, "RAY TRACING", info.rayTracing ? "ON" : "OFF", info.rayTracing ? on : off, 1.0f);
			y += rowH;
			statRow(innerX, y, innerW, "DLSS", "ULTRA QUALITY", white, 0.9f);
			y += rowH + 22.0f;

			// -------- MATCH: these ones are real, straight from the engine
			panel(x, y - 8.0f, boxW, 6.0f + rowH * 5.0f, "MATCH", p, 1.0f);
			y += 10.0f;
			statRow(innerX, y, innerW, "MAP", info.mapName, p.accent, 1.0f);
			y += rowH;
			statRow(innerX, y, innerW, "OBJECTS", std::to_string(info.objectCount), white, 0.9f);
			y += rowH;
			statRow(innerX, y, innerW, "CAMERA", info.cameraMode + " x" + fixed(info.cameraZoom, 2), white, 0.9f);
			y += rowH;
			statRow(innerX, y, innerW, "THEME", info.themeName, p.accent2, 1.0f);
			y += rowH;
			statRow(innerX, y, innerW, "SESSION", clock(info.sessionSeconds), white, 0.9f);
		}

		// ------------------------------------------------------------------ bottom-left
		void drawProgression(const HudInfo& info, const GamerTheme::Theme& p, float h) {
			const float slide = (1.0f - easeOut(info.reveal)) * 120.0f;
			const float y = h - 50.0f + slide;
			const float cx = margin + 20.0f;
			const float cy = y + 18.0f;

			// Rank emblem: a glowing diamond with the level number in it
			Render2D::blendAdditive();
			Render2D::radialGlow(cx, cy, 30.0f, p.accent, 0.25f, p.accent, 0.0f, 24);
			Render2D::blendNormal();
			glColor4f(p.accent.r * 0.35f, p.accent.g * 0.35f, p.accent.b * 0.35f, 0.95f);
			glBegin(GL_QUADS);
			glVertex2f(cx, cy - 18.0f);
			glVertex2f(cx + 18.0f, cy);
			glVertex2f(cx, cy + 18.0f);
			glVertex2f(cx - 18.0f, cy);
			glEnd();
			glLineWidth(1.5f);
			glColor4f(p.accent.r, p.accent.g, p.accent.b, 1.0f);
			glBegin(GL_LINE_LOOP);
			glVertex2f(cx, cy - 18.0f);
			glVertex2f(cx + 18.0f, cy);
			glVertex2f(cx, cy + 18.0f);
			glVertex2f(cx - 18.0f, cy);
			glEnd();
			const std::string lvl = std::to_string(info.level);
			Render2D::text(cx - Render2D::textWidth(lvl, Font::Body) * 0.5f, cy + 5.0f, lvl, Font::Body, white, 1.0f);

			// LVL + rank name, XP bar, XP numbers
			const float tx = cx + 28.0f;
			Render2D::textShadow(tx, y + 10.0f, "LVL " + lvl, Font::Body, white, 1.0f);
			Render2D::textGlow(tx + Render2D::textWidth("LVL " + lvl, Font::Body) + 8.0f, y + 10.0f, info.rank, Font::Tiny, p.accent, p.accent, 1.0f);

			const float span = static_cast<float>(std::max(1, info.xpLevelEnd - info.xpLevelStart));
			const float progress = (info.xp - info.xpLevelStart) / span;
			Render2D::segmentBar(tx, y + 16.0f, 180.0f, 6.0f, progress, 15, p.accent, 1.0f);

			Render2D::text(tx, y + 34.0f, std::to_string(info.xp - info.xpLevelStart) + " / " + std::to_string(info.xpLevelEnd - info.xpLevelStart) + " XP", Font::Tiny, white, 0.6f);

			if (!info.isGameMap) {
				Render2D::text(tx + 104.0f, y + 34.0f, "[F1] CONTROLS   [TAB] NEXT MAP", Font::Tiny, p.accent2, 0.75f);
			}
		}

		// ------------------------------------------------------------------ top-center
		void drawScoreboard(const HudInfo& info, const GamerTheme::Theme& p, float w) {
			// CHUCK [score] | LIVE 00:42 | [score] BUGS
			const float sideW = 62.0f;
			const float midW = 58.0f;
			const float total = sideW * 2.0f + midW;
			const float x = (w - total) * 0.5f;
			const float slide = (1.0f - easeOut(info.reveal)) * -80.0f;
			const float y = 16.0f + slide;
			const float hgt = 28.0f;

			Render2D::chamferBox(x, y, sideW, hgt, 6.0f, GamerTheme::scale(p.accent, 0.35f), 0.85f);
			Render2D::rect(x + sideW, y, midW, hgt, black, 0.7f);
			Render2D::chamferBox(x + sideW + midW, y, sideW, hgt, 6.0f, GamerTheme::scale(p.accent2, 0.35f), 0.85f);

			Render2D::text(x + 6.0f, y + 12.0f, "CHUCK", Font::Tiny, p.accent, 1.0f);
			const std::string cs = std::to_string(info.chuckScore);
			Render2D::strokeTextNeon(x + sideW - 8.0f - Render2D::strokeTextWidth(cs, 14.0f), y + 24.0f, cs, 14.0f, p.accent, 1.0f);

			const float bx = x + sideW + midW;
			Render2D::text(bx + sideW - 6.0f - Render2D::textWidth("BUGS", Font::Tiny), y + 12.0f, "BUGS", Font::Tiny, p.accent2, 1.0f);
			const std::string bs = std::to_string(info.bugsScore);
			Render2D::strokeTextNeon(bx + 8.0f, y + 24.0f, bs, 14.0f, p.accent2, 1.0f);

			// LIVE dot blinks, match clock counts up
			const float mx = x + sideW;
			const Rgb liveRed = { 1.0f, 0.2f, 0.25f };
			const float blink = std::fmod(info.time, 1.2f) < 0.8f ? 1.0f : 0.25f;
			Render2D::rect(mx + 10.0f, y + 6.0f, 5.0f, 5.0f, liveRed, blink);
			Render2D::text(mx + 19.0f, y + 12.0f, "LIVE", Font::Tiny, liveRed, 0.9f);
			const int secs = static_cast<int>(info.sessionSeconds);
			std::ostringstream clockText;
			clockText << std::setfill('0') << std::setw(2) << (secs / 60) % 100 << ":" << std::setw(2) << secs % 60;
			Render2D::text(mx + (midW - Render2D::textWidth(clockText.str(), Font::MonoSmall)) * 0.5f, y + 25.0f, clockText.str(), Font::MonoSmall, white, 0.95f);
		}

		// ------------------------------------------------------------------ frame
		void drawFrame(const HudInfo& info, const GamerTheme::Theme& p, float w, float h) {
			// Sci-fi corner brackets hugging the viewport, breathing slowly
			const float breathe = 0.25f + 0.1f * std::sin(info.time * 1.3f);
			Render2D::cornerBrackets(6.0f, 6.0f, w - 12.0f, h - 12.0f, 26.0f, p.accent, breathe * info.reveal, 2.0f);
		}
	}

	void drawControls(const GamerTheme::Theme& p, float time, float alpha) {
		if (alpha <= 0.0f) {
			return;
		}

		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());

		// Dim the whole game behind the sheet
		Render2D::rect(0.0f, 0.0f, w, h, black, 0.55f * alpha);

		const float boxW = std::min(w - 40.0f, 600.0f);
		const float boxH = std::min(h - 40.0f, 440.0f);
		const float x = (w - boxW) * 0.5f;
		const float y = (h - boxH) * 0.5f + (1.0f - easeOut(alpha)) * 30.0f;

		Render2D::chamferBox(x, y, boxW, boxH, 18.0f, GamerTheme::scale(p.skyTop, 0.55f), 0.92f * alpha);
		Render2D::chamferOutline(x, y, boxW, boxH, 18.0f, p.accent, 0.8f * alpha, 1.5f);
		Render2D::cornerBrackets(x - 6.0f, y - 6.0f, boxW + 12.0f, boxH + 12.0f, 18.0f, p.accent2, 0.6f * alpha, 2.0f);

		const std::string title = "CONTROLS";
		const float titleW = Render2D::strokeTextWidth(title, 24.0f, 6.0f);
		Render2D::strokeTextNeon(x + (boxW - titleW) * 0.5f, y + 42.0f, title, 24.0f, p.accent, alpha, 6.0f);

		struct Binding {
			const char* key;
			const char* action;
		};

		// Left column: the original homework controls + the game. Right: Chuck Edition.
		static const Binding homework[] = {
			{ "1", "Wireframe" },
			{ "2", "Solid" },
			{ "3", "Toggle axis display" },
			{ "4", "Bend the elbows (robot)" },
			{ "F4", "Cloak the robot" },
			{ "L-CLICK", "Grayscale toggle" },
			{ "ESC", "Rage quit" },
		};
		static const Binding world[] = {
			{ "WASD", "Move (camera relative)" },
			{ "SPACE", "Jump" },
			{ "Q / E", "Rotate camera" },
			{ "R", "Restart round" },
		};
		static const Binding chuck[] = {
			{ "F1", "This panel" },
			{ "F2", "Cycle RGB theme" },
			{ "F3", "Stats panels" },
			{ "F5", "Cinematic mode" },
			{ "F6", "Neon outlines" },
			{ "F7", "Ray tracing (allegedly)" },
			{ "F8", "CRT filter" },
			{ "F9", "Clip it! (screenshot)" },
			{ "F10", "Scenery" },
			{ "F11", "Fullscreen" },
			{ "F12", "Photo mode" },
			{ "TAB", "Next map" },
			{ "R-DRAG", "Orbit camera" },
			{ "SCROLL", "Zoom" },
		};

		const float colW = (boxW - 60.0f) * 0.5f;
		const float rowH = 18.0f;

		const auto section = [&](float sx, float sy, const char* heading, const Binding* rows, int count) {
			Render2D::text(sx, sy, heading, Font::Tiny, p.accent2, alpha);
			Render2D::line(sx, sy + 5.0f, sx + colW, sy + 5.0f, p.accent2, 0.4f * alpha, 1.0f);
			float ry = sy + 22.0f;
			for (int i = 0; i < count; ++i) {
				// key "keycap" + action label
				const float kw = static_cast<float>(Render2D::textWidth(rows[i].key, Font::MonoSmall)) + 10.0f;
				Render2D::chamferBox(sx, ry - 11.0f, kw, 15.0f, 3.0f, p.accent, 0.18f * alpha);
				Render2D::chamferOutline(sx, ry - 11.0f, kw, 15.0f, 3.0f, p.accent, 0.6f * alpha);
				Render2D::text(sx + 5.0f, ry, rows[i].key, Font::MonoSmall, white, alpha);
				Render2D::text(sx + 78.0f, ry, rows[i].action, Font::Body, white, 0.85f * alpha);
				ry += rowH;
			}
			return ry;
		};

		const float leftX = x + 24.0f;
		const float rightX = x + 36.0f + colW;
		const float top = y + 74.0f;

		float leftY = section(leftX, top, "HOMEWORK (SIMON'S ORIGINAL CONTROLS)", homework, 7);
		section(leftX, leftY + 10.0f, "CHUCK WORLD (TAB TO GET THERE)", world, 4);
		section(rightX, top, "CHUCK SPECIAL EDITION", chuck, 14);

		const std::string footer = "PSST... THE KONAMI CODE STILL WORKS IN 2026";
		const float fw = static_cast<float>(Render2D::textWidth(footer, Font::Tiny));
		const float blink = 0.45f + 0.35f * std::sin(time * 3.0f);
		Render2D::text(x + (boxW - fw) * 0.5f, y + boxH - 16.0f, footer, Font::Tiny, p.accent2, blink * alpha);
	}

	void drawConfetti(float age) {
		if (age < 0.0f || age > 5.0f) {
			return;
		}

		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());
		const float fade = std::min(1.0f, (5.0f - age) / 1.0f);

		// 160 pieces, every one a pure function of (index, age): launched from above the
		// screen with a random sideways drift, falling with gravity and fluttering as it spins
		for (int i = 0; i < 160; ++i) {
			const float r1 = std::fmod(std::sin(i * 12.9898f) * 43758.5453f, 1.0f);
			const float r2 = std::fmod(std::sin(i * 78.233f) * 12345.678f, 1.0f);
			const float a = std::fabs(r1);
			const float b = std::fabs(r2);

			const float x0 = a * w;
			const float y0 = -b * h * 0.4f - 10.0f;
			const float vx = (b - 0.5f) * 140.0f;
			const float vy = 40.0f + a * 120.0f;

			const float px = x0 + vx * age + std::sin(age * (3.0f + a * 4.0f) + i) * 22.0f;
			const float py = y0 + vy * age + 0.5f * 160.0f * age * age;
			if (py > h + 20.0f) {
				continue;
			}

			const float spin = age * (4.0f + b * 8.0f) + i;
			const float half = 3.0f + a * 3.0f;
			const float flip = std::fabs(std::cos(spin)); // paper turning edge-on
			const Rgb c = GamerTheme::hsv(a + age * 0.2f, 0.85f, 1.0f);

			glColor4f(c.r, c.g, c.b, 0.95f * fade);
			glBegin(GL_QUADS);
			const float cs = std::cos(spin * 0.7f);
			const float sn = std::sin(spin * 0.7f);
			const float hx = half;
			const float hy = half * 0.55f * (0.2f + 0.8f * flip);
			glVertex2f(px + (-hx * cs - -hy * sn), py + (-hx * sn + -hy * cs));
			glVertex2f(px + (hx * cs - -hy * sn), py + (hx * sn + -hy * cs));
			glVertex2f(px + (hx * cs - hy * sn), py + (hx * sn + hy * cs));
			glVertex2f(px + (-hx * cs - hy * sn), py + (-hx * sn + hy * cs));
			glEnd();
		}
	}

	void draw(const HudInfo& info, const GamerTheme::Theme& palette) {
		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());

		drawFrame(info, palette, w, h);
		drawLogo(info, palette);
		drawPerformance(info, palette, w);
		if (info.showStats) {
			drawStats(info, palette);
		}
		if (!info.isGameMap) {
			drawScoreboard(info, palette, w);
		}
		drawProgression(info, palette, h);
	}
}
