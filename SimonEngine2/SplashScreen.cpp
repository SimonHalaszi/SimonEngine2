#include "SplashScreen.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <string>

#include "Render2D.hpp"

namespace SplashScreen {

	using GamerTheme::Rgb;
	using Render2D::Font;

	namespace {
		const Rgb white = { 1.0f, 1.0f, 1.0f };
		const Rgb okGreen = { 0.35f, 1.0f, 0.45f };
		const Rgb warnYellow = { 1.0f, 0.85f, 0.2f };

		struct BootLine {
			const char* text;
			const char* status;
			bool warning; // yellow instead of green
		};

		// -----------------------------------------------------------------------------
		//  The boot log. Every line is a lie except the last one.
		// -----------------------------------------------------------------------------
		const BootLine bootLines[] = {
			{ "Initializing RGB subsystem",               "OK",      false },
			{ "Overclocking frame pacer to 244 Hz",       "OK",      false },
			{ "Downloading more RAM",                     "OK",      false },
			{ "Compiling 4,096 shaders",                  "SKIPPED", true  }, // (there are zero shaders)
			{ "Calibrating gamer chair lumbar support",   "OK",      false },
			{ "Enabling RTX (Real Triangles eXtreme)",    "OK",      false },
			{ "Patching Simon's bugs",                    "87%",     true  },
			{ "Loading CHUCK SPECIAL EDITION",            "OK",      false },
		};
		const int bootLineCount = static_cast<int>(sizeof(bootLines) / sizeof(bootLines[0]));

		constexpr float firstLineAt = 0.3f;
		constexpr float lineInterval = 0.38f;

		float clamp01(float v) {
			return std::min(std::max(v, 0.0f), 1.0f);
		}

		// Progress bar that fills fast, slows down, then sits at 99% for an agonizing
		// moment before snapping to 100. Every installer ever.
		float fakeProgress(float t) {
			const float end = firstLineAt + lineInterval * bootLineCount;
			if (t < end - 0.5f) {
				const float k = clamp01((t - firstLineAt) / (end - 0.5f - firstLineAt));
				return 0.99f * (1.0f - (1.0f - k) * (1.0f - k));
			}
			return t < end ? 0.99f : 1.0f;
		}

		// The logo "warms up" like a real neon tube: a few random-looking flickers, then on
		float neonWarmup(float t) {
			if (t > 0.7f) {
				return 1.0f;
			}
			const float flicker = std::sin(t * 61.0f) * std::sin(t * 23.0f);
			return flicker > 0.1f ? 0.25f + t : 0.05f;
		}
	}

	void draw(const GamerTheme::Theme& p, float t, float alpha) {
		if (alpha <= 0.0f) {
			return;
		}

		const float w = static_cast<float>(Render2D::width());
		const float h = static_cast<float>(Render2D::height());
		const float cx = w * 0.5f;

		// ---------------------------------------------------------------- background
		Render2D::rectGradientV(0.0f, 0.0f, w, h, GamerTheme::scale(p.skyTop, 0.8f), alpha, GamerTheme::scale(p.skyHorizon, 0.45f), alpha);

		// Retro perspective floor lines converging on a vanishing point at the center
		Render2D::blendAdditive();
		const float horizon = h * 0.62f;
		for (int i = -12; i <= 12; ++i) {
			Render2D::line(cx + i * 6.0f, horizon, cx + i * 90.0f, h, p.grid, 0.18f * alpha, 1.0f);
		}
		for (int i = 0; i < 9; ++i) {
			// horizontal lines bunch up toward the horizon and scroll toward the viewer
			const float k = std::fmod(i / 9.0f + t * 0.25f, 1.0f);
			const float y = horizon + (h - horizon) * k * k;
			Render2D::line(0.0f, y, w, y, p.grid, 0.25f * k * alpha, 1.0f);
		}
		Render2D::rect(0.0f, horizon, w, 1.0f, p.accent2, 0.6f * alpha);
		Render2D::blendNormal();

		// ---------------------------------------------------------------- logo
		const float glow = neonWarmup(t) * alpha;
		const std::string logo = "SIMONENGINE2";
		const float logoCap = std::min(48.0f, w / 13.0f);
		const float logoW = Render2D::strokeTextWidth(logo, logoCap, 4.0f);
		Render2D::strokeTextNeon(cx - logoW * 0.5f, h * 0.30f, logo, logoCap, p.accent, glow, 4.0f);

		const std::string edition = "C H U C K    S P E C I A L    E D I T I O N";
		const float editionW = static_cast<float>(Render2D::textWidth(edition, Font::Large));
		Render2D::textGlow(cx - editionW * 0.5f, h * 0.30f + 34.0f, edition, Font::Large, white, p.accent2, clamp01((t - 0.5f) * 3.0f) * alpha);

		// A glowing divider that draws itself outward from the center
		const float dividerW = std::min(w * 0.7f, 520.0f) * clamp01((t - 0.4f) * 2.0f);
		Render2D::blendAdditive();
		Render2D::rectGradientH(cx - dividerW * 0.5f, h * 0.30f + 48.0f, dividerW * 0.5f, 2.0f, p.accent, 0.0f, p.accent, alpha);
		Render2D::rectGradientH(cx, h * 0.30f + 48.0f, dividerW * 0.5f, 2.0f, p.accent, alpha, p.accent, 0.0f);
		Render2D::blendNormal();

		// ---------------------------------------------------------------- boot log
		const float logW = std::min(w * 0.8f, 440.0f);
		const float logX = cx - logW * 0.5f;
		float logY = h * 0.30f + 78.0f;
		const int visible = std::min(bootLineCount, static_cast<int>((t - firstLineAt) / lineInterval) + 1);

		for (int i = 0; i < visible && t >= firstLineAt; ++i) {
			const BootLine& line = bootLines[i];
			const float lineAge = t - (firstLineAt + i * lineInterval);
			const float lineAlpha = clamp01(lineAge * 6.0f) * alpha;

			Render2D::text(logX, logY, std::string("> ") + line.text, Font::MonoSmall, white, 0.8f * lineAlpha);

			// The status shows up a beat after the line itself (dots "working" until then)
			const bool done = lineAge > lineInterval * 0.6f;
			const std::string status = done ? std::string("[ ") + line.status + " ]" : std::string(static_cast<std::size_t>(1 + static_cast<int>(lineAge * 12.0f) % 3), '.');
			const Rgb statusColor = line.warning ? warnYellow : okGreen;
			const float sw = static_cast<float>(Render2D::textWidth(status, Font::MonoSmall));
			Render2D::text(logX + logW - sw, logY, status, Font::MonoSmall, done ? statusColor : white, lineAlpha);

			logY += 16.0f;
		}

		// ---------------------------------------------------------------- progress bar
		const float progress = fakeProgress(t);
		const float barY = h * 0.30f + 78.0f + bootLineCount * 16.0f + 14.0f;
		Render2D::progressBar(logX, barY, logW, 6.0f, progress, p.accent, white, alpha);
		const std::string percent = std::to_string(static_cast<int>(progress * 100.0f)) + "%";
		Render2D::text(logX + logW - Render2D::textWidth(percent, Font::MonoSmall), barY + 20.0f, percent, Font::MonoSmall, p.accent, alpha);
		Render2D::text(logX, barY + 20.0f, progress < 1.0f ? "LOADING ASSETS" : "READY", Font::MonoSmall, white, 0.6f * alpha);

		// ---------------------------------------------------------------- press any key
		if (t > 3.6f && std::fmod(t, 0.9f) < 0.6f) {
			const std::string press = "PRESS ANY KEY";
			const float pw = Render2D::strokeTextWidth(press, 14.0f, 3.0f);
			Render2D::strokeTextNeon(cx - pw * 0.5f, h * 0.88f, press, 14.0f, p.accent2, alpha, 3.0f);
		}

		const std::string credits = "POWERED BY SIMONENGINE2 (SIMON HALASZI)  //  SPECIAL EDITION BY CHUCK";
		const float creditsW = static_cast<float>(Render2D::textWidth(credits, Font::Tiny));
		Render2D::text(cx - creditsW * 0.5f, h - 14.0f, credits, Font::Tiny, white, 0.4f * alpha);
	}
}
