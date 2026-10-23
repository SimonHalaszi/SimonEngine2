#include "ChuckEdition.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Engine.hpp"
#include "Scene.hpp"
#include "InputManager.hpp"
#include "Render2D.hpp"
#include "StudioLighting.hpp"
#include "Backdrop.hpp"
#include "NeonWorld.hpp"
#include "Hud.hpp"
#include "SplashScreen.hpp"
#include "PostFX.hpp"
#include "Screenshot.hpp"

#include "RobotScene.hpp"
#include "ObjScene.hpp"
#include "MagicCubeScene.hpp"
#include "ChuckWorldScene.hpp"

namespace {
	// How long a theme crossfade takes, in seconds. Fast enough to feel snappy,
	// slow enough that you SEE the whole engine melt from one palette into the next.
	constexpr float themeFadeSeconds = 0.6f;

	// FPS is averaged over windows of this many seconds so the number is readable
	// instead of flickering like a slot machine.
	constexpr float fpsWindowSeconds = 0.5f;

	// TAB map transition timing: the wipe covers the screen, the map swaps while hidden,
	// then the wipe uncovers the new map
	constexpr float wipeCoverSeconds = 0.32f;
	constexpr float wipeTotalSeconds = 0.80f;
	constexpr int mapCount = 4;

	// The original window title, kept so the homework grader still knows whose engine this is
	const char* const homeworkSignature = "(Simon Halaszi) (811196947)";

	float smoothstep01(float t) {
		t = std::min(std::max(t, 0.0f), 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}
}

// ============================================================================ boot

void ChuckEdition::init() {
	printConsoleBanner();
	updateWindowTitle(nullptr);
}

void ChuckEdition::printConsoleBanner() const {
	// Plain escaped literals (not a raw string) so no compiler ever mistakes the
	// trailing backslashes in the art for line continuations.
	static const char* const banner[] = {
		"  ____  _                       _____             _            ____",
		" / ___|(_)_ __ ___   ___  _ __ | ____|_ __   __ _(_)_ __   ___|___ \\",
		" \\___ \\| | '_ ` _ \\ / _ \\| '_ \\|  _| | '_ \\ / _` | | '_ \\ / _ \\ __) |",
		"  ___) | | | | | | | (_) | | | | |___| | | | (_| | | | | |  __// __/",
		" |____/|_|_| |_| |_|\\___/|_| |_|_____|_| |_|\\__, |_|_| |_|\\___|_____|",
		"                                            |___/",
		"              >>>  C H U C K   S P E C I A L   E D I T I O N  <<<",
		"                      \"SimonEngine2, but better.\"  v2.0.69-PRO",
	};

	std::cout << "\n";
	for (const char* line : banner) {
		std::cout << line << "\n";
	}
	std::cout << "\n";

	std::cout << "  [ OK ] RGB subsystem online (" << GamerTheme::themeCount() << " themes loaded)\n";
	std::cout << "  [ OK ] Frame pacer overclocked\n";
	std::cout << "  [ OK ] Gamer posture calibrated\n";
	std::cout << "  [ OK ] Homework mode preserved: " << homeworkSignature << "\n\n";
	std::cout << "  F1  controls        F2  RGB theme       F3  stats          F5  cinematic\n";
	std::cout << "  F6  neon outlines   F7  ray tracing     F8  CRT filter     F9  clip it\n";
	std::cout << "  F10 scenery         F11 fullscreen      F12 photo mode     TAB next map\n";
	std::cout << "  R-DRAG orbit        SCROLL zoom         ESC rage quit\n\n";
}

// ============================================================================ per-tick

void ChuckEdition::update(Scene* scene) {
	InputManager& input = InputManager::getInstance();

	const float tickNow = time();
	const float dt = lastUpdateTime_ < 0.0f ? 0.0f : std::min(tickNow - lastUpdateTime_, 0.05f);
	lastUpdateTime_ = tickNow;

	// While the splash screen is up it eats every keypress, so nothing below runs
	if (updateSplash()) {
		return;
	}

	updateCameraControls(scene, dt);
	updateSecrets();

	updateMapTransition();

	// Achievements, callouts, XP, the scoreboard... all the dopamine
	observeHomeworkInput(scene);
	updateFakeKillfeed();
	notifications_.update(time());

	// TAB = next map (ASCII 9)
	if (scene && input.isPressed(9) && transitionStart_ < 0.0f) {
		startMapTransition(*scene);
	}

	if (input.isSpecialKeyPressed(GLUT_KEY_F2)) {
		cycleTheme();
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F10)) {
		sceneryEnabled_ = !sceneryEnabled_;
		callout("SCENERY", sceneryEnabled_ ? "ON" : "OFF");
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F3)) {
		statsVisible_ = !statsVisible_;
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F6)) {
		outlinesEnabled_ = !outlinesEnabled_;
		callout("NEON OUTLINES", outlinesEnabled_ ? "ON" : "OFF");
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F7)) {
		lightingEnabled_ = !lightingEnabled_;
		std::cout << "  [RTX] Ray tracing " << (lightingEnabled_ ? "ON (trust me bro)" : "OFF (console mode)") << "\n";
		callout("RAY TRACING", lightingEnabled_ ? "ON  (TRUST ME BRO)" : "OFF  (CONSOLE MODE)");
		if (!lightingEnabled_) {
			unlock(Achievement::ConsolePeasant);
		}
	}

	if (input.isSpecialKeyPressed(GLUT_KEY_F1)) {
		helpVisible_ = !helpVisible_;
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F5)) {
		cinematic_ = !cinematic_;
		callout(cinematic_ ? "CINEMATIC MODE" : "GAMEPLAY MODE", cinematic_ ? "ROLL CAMERA" : "BACK IN CONTROL");
		if (cinematic_) {
			unlock(Achievement::Directors);
		}
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F8)) {
		crtEnabled_ = !crtEnabled_;
		callout(crtEnabled_ ? "CRT MODE" : "CRT OFF", crtEnabled_ ? "1998 CALLED, IT WANTS ITS MONITOR BACK" : "WELCOME BACK TO FLAT PANELS");
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F9)) {
		screenshotRequested_ = true;
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F11)) {
		glutFullScreenToggle();
	}
	if (input.isSpecialKeyPressed(GLUT_KEY_F12)) {
		photoMode_ = !photoMode_;
		std::cout << "  [PHOTO] Photo mode " << (photoMode_ ? "ON (F12 to bring the HUD back)" : "OFF") << "\n";
	}

	// Passive XP: +1 every two seconds, just for showing up. That's how gaming works.
	const float now = time();
	if (now - lastPassiveXp_ >= 2.0f) {
		lastPassiveXp_ = now;
		awardXp(1);
	}
	if (now > 300.0f) {
		unlock(Achievement::TouchGrass);
	}
}

// ============================================================================ secrets

namespace {
	// Konami inputs: arrows are encoded as 1000 + GLUT key code so they can't collide
	// with the plain ASCII 'b' and 'a' at the end
	const int konamiSequence[] = {
		1000 + GLUT_KEY_UP, 1000 + GLUT_KEY_UP, 1000 + GLUT_KEY_DOWN, 1000 + GLUT_KEY_DOWN,
		1000 + GLUT_KEY_LEFT, 1000 + GLUT_KEY_RIGHT, 1000 + GLUT_KEY_LEFT, 1000 + GLUT_KEY_RIGHT,
		'b', 'a'
	};
	const int konamiLength = static_cast<int>(sizeof(konamiSequence) / sizeof(konamiSequence[0]));

	bool endsWith(const std::string& s, const char* suffix) {
		const std::string tail(suffix);
		return s.size() >= tail.size() && s.compare(s.size() - tail.size(), tail.size(), tail) == 0;
	}
}

void ChuckEdition::updateSecrets() {
	InputManager& input = InputManager::getInstance();

	static const int arrows[] = { GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_LEFT, GLUT_KEY_RIGHT };
	for (int key : arrows) {
		if (input.isSpecialKeyPressed(key)) {
			advanceKonami(1000 + key);
		}
	}

	for (char c : input.getTypedChars()) {
		const char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
		advanceKonami(lower);

		typedHistory_ += lower;
		if (typedHistory_.size() > 16) {
			typedHistory_.erase(0, typedHistory_.size() - 16);
		}
		checkTypedWord();
	}
}

void ChuckEdition::advanceKonami(int inputCode) {
	if (inputCode == konamiSequence[konamiProgress_]) {
		++konamiProgress_;
		if (konamiProgress_ == konamiLength) {
			konamiProgress_ = 0;
			activateChuckMode();
		}
	}
	else {
		// A wrong input resets, but if it was itself a valid opener, count it
		konamiProgress_ = (inputCode == konamiSequence[0]) ? 1 : 0;
	}
}

void ChuckEdition::activateChuckMode() {
	fromTheme_ = palette_;
	themeSwitchTime_ = time();
	chuckMode_ = true;
	confettiStart_ = time();

	callout("CHUCK MODE", "ACTIVATED  //  ALL RGB UNLOCKED  //  F2 TO RETURN TO MORTALITY");
	killfeed("Chuck", "KONAMI", "Everyone");
	unlock(Achievement::Konami);
	std::cout << "  [????] CHUCK MODE ACTIVATED. It's over 9000.\n";
}

void ChuckEdition::checkTypedWord() {
	// Type these words anywhere in game. Not documented anywhere. Except here. Hi.
	if (endsWith(typedHistory_, "simon")) {
		callout("SIMON DETECTED", "SKILL ISSUE INCOMING");
		killfeed("Chuck", "OWNED", "Simon");
		++chuckScore_;
	}
	else if (endsWith(typedHistory_, "chuck")) {
		callout("THE GOAT", "HAS ENTERED THE CHAT");
	}
	else if (endsWith(typedHistory_, "rtx")) {
		callout("RTX ON", "(ALLEGEDLY)");
	}
	else if (endsWith(typedHistory_, "fps")) {
		callout(std::to_string(static_cast<int>(fps_ * 4.0f)) + " FPS", "FRAME GEN DOES COUNT. IT DOES. SHUT UP.");
	}
	else if (endsWith(typedHistory_, "lag")) {
		callout("LAG?", "PING IS 1MS. THAT'S A SKILL ISSUE.");
	}
	else if (endsWith(typedHistory_, "gg")) {
		callout("GG", "EZ");
	}
}

void ChuckEdition::updateCameraControls(Scene* scene, float dt) {
	// Chuck World has a proper third-person rig of its own; only drive the homework cameras
	if (!scene || dynamic_cast<ChuckWorldScene*>(scene)) {
		return;
	}

	InputManager& input = InputManager::getInstance();
	Camera& camera = scene->getCamera();
	const EngineMath::Vector3 worldUp(0.0f, 1.0f, 0.0f);

	float yaw = 0.0f;
	float pitch = 0.0f;

	// Right-drag = orbit. (Right click used to open Simon's HW1 menus; HW2 retired those.)
	if (input.isMouseButtonDown(MOUSEBUTTON_RIGHT)) {
		yaw -= input.mouseDeltaX() * 0.01f;
		pitch += input.mouseDeltaY() * 0.01f;
	}

	// Cinematic mode: a slow, steady turntable orbit. Every game trailer ever.
	if (cinematic_) {
		yaw += 0.35f * dt;
	}

	if (yaw != 0.0f) {
		camera.pivotAroundTarget(worldUp, yaw);
	}

	if (pitch != 0.0f) {
		// Tilt around the camera's horizontal axis. Clamp the elevation so you can't flip
		// over the top (gluLookAt goes haywire when you look straight down the up vector)
		const EngineMath::Vector3 offset = camera.getPosition() - camera.getTarget();
		const float distance = offset.length();
		if (distance > 1e-4f) {
			const float elevation = std::asin(std::min(1.0f, std::max(-1.0f, offset.y_ / distance)));
			const float maxElevation = EngineMath::degreesToRadians(82.0f);
			const float minElevation = EngineMath::degreesToRadians(-25.0f);
			const float clamped = std::min(maxElevation, std::max(minElevation, elevation + pitch)) - elevation;
			const EngineMath::Vector3 axis = offset.cross(worldUp).normalized();
			if (axis.lengthSquared() > 0.5f && clamped != 0.0f) {
				camera.pivotAroundTarget(axis, clamped);
			}
		}
	}

	// Scroll = zoom (same step sizes Simon's HW1 scenes used)
	const bool perspective = camera.getProjectionMode() == Camera::ProjectionMode::Perspective;
	const double zoomStep = perspective ? 2.0 : 0.05;
	if (input.isMouseButtonPressed(MOUSEBUTTON_SCROLLUP)) {
		camera.zoomIn(zoomStep);
	}
	if (input.isMouseButtonPressed(MOUSEBUTTON_SCROLLDOWN)) {
		camera.zoomOut(zoomStep);
	}
}

void ChuckEdition::takeScreenshot() {
	screenshotRequested_ = false;
	const std::string file = Screenshot::saveBmp();
	if (file.empty()) {
		callout("CLIP FAILED", "COULD NOT WRITE THE FILE");
		return;
	}
	flashStart_ = time();
	std::cout << "  [CLIP] Saved " << file << "\n";
	callout("CLIPPED!", file);
	unlock(Achievement::Shutterbug);
}

bool ChuckEdition::updateSplash() {
	if (splashEndTime_ >= 0.0f) {
		return false;
	}

	const float now = time();
	const bool skipped = anyInputPressed();
	if (skipped || now >= SplashScreen::autoContinueSeconds) {
		splashEndTime_ = now;
		if (skipped) {
			// Swallow the key that skipped the splash, so pressing '1' to skip doesn't also
			// flip the robot into wireframe the same instant
			InputManager::getInstance().reset();
			std::cout << "  [BOOT] Splash skipped. Impatient. Respect.\n";
		}
		return true;
	}
	return true;
}

bool ChuckEdition::anyInputPressed() {
	const InputManager& input = InputManager::getInstance();
	if (!input.getTypedChars().empty()) {
		return true;
	}
	if (input.isMouseButtonPressed(MOUSEBUTTON_LEFT) || input.isMouseButtonPressed(MOUSEBUTTON_MIDDLE) || input.isMouseButtonPressed(MOUSEBUTTON_RIGHT)) {
		return true;
	}
	static const int specialKeys[] = {
		GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
		GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11, GLUT_KEY_F12,
		GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN
	};
	for (int key : specialKeys) {
		if (input.isSpecialKeyPressed(key)) {
			return true;
		}
	}
	return false;
}

float ChuckEdition::hudReveal() const {
	if (splashEndTime_ < 0.0f) {
		return 0.0f;
	}
	return std::min(1.0f, std::max(0.0f, (time() - splashEndTime_ - 0.15f) / 0.9f));
}

void ChuckEdition::observeHomeworkInput(const Scene* scene) {
	// Chuck Edition never CHANGES what the homework keys do; it just watches them and
	// throws a parade every time one gets pressed
	InputManager& input = InputManager::getInstance();
	const float now = time();

	if (!sawAnyInput_ && (!input.getTypedChars().empty() || input.isMouseButtonPressed(MOUSEBUTTON_LEFT))) {
		sawAnyInput_ = true;
		unlock(Achievement::FirstBlood);
	}

	if (scene) {
		const unsigned int bit = 1u << mapIndex(*scene);
		if (!(mapsSeen_ & bit)) {
			mapsSeen_ |= bit;
			if (mapsSeen_ == (1u << mapCount) - 1u) {
				unlock(Achievement::WorldTourist);
			}
		}
	}

	if (input.isPressed('1')) {
		callout("WIREFRAME", "X-RAY VISION ENGAGED");
		unlock(Achievement::XRayVision);
	}
	if (input.isPressed('2')) {
		callout("SOLID", "BACK TO REALITY");
	}
	if (input.isPressed('3') && scene && !dynamic_cast<const ChuckWorldScene*>(scene)) {
		callout("AXIS DISPLAY", "TOGGLED");
	}
	if (input.isMouseButtonPressed(MOUSEBUTTON_LEFT)) {
		grayscale_ = !grayscale_;
		callout(grayscale_ ? "MONOCHROME" : "FULL COLOR", grayscale_ ? "FILM NOIR MODE" : "RGB RESTORED");
		if (grayscale_) {
			unlock(Achievement::FilmNoir);
		}
	}

	// '4' (elbows) and F4 (cloak) are robot-only homework controls
	if (scene && dynamic_cast<const RobotScene*>(scene)) {
		if (input.isPressed('4')) {
			++elbowBends_;
			unlock(Achievement::ElbowGrease);
			if (elbowBends_ >= 18) {
				unlock(Achievement::NoodleArms);
			}
		}
		if (input.isSpecialKeyPressed(GLUT_KEY_F4)) {
			callout("CLOAK", "ROBOT VISIBILITY TOGGLED");
			unlock(Achievement::NowYouSeeMe);
		}
	}
	(void)now;
}

void ChuckEdition::updateFakeKillfeed() {
	const float now = time();
	if (now < nextFakeKill_) {
		return;
	}

	// ---------------------------------------------------------------------------
	//  100% organic, free-range, totally real killfeed events.
	//  Chuck farms bugs. The bugs occasionally farm Simon. The scoreboard keeps track.
	// ---------------------------------------------------------------------------
	struct Kill {
		const char* killer;
		const char* weapon;
		const char* victim;
		bool chuckScores;
	};
	static const Kill kills[] = {
		{ "Chuck",      "HEADSHOT", "Segfault",           true },
		{ "Chuck",      "NOSCOPE",  "Memory Leak",        true },
		{ "Chuck",      "360",      "Off-By-One Error",   true },
		{ "Chuck",      "AWP",      "Null Pointer",       true },
		{ "Chuck",      "KNIFE",    "Linker Error",       true },
		{ "Chuck",      "CLUTCH",   "Race Condition",     true },
		{ "Chuck",      "WALLBANG", "Undefined Behavior", true },
		{ "Chuck",      "ACE",      "Merge Conflict",     true },
		{ "Quaternion", "HAMILTON", "Gimbal Lock",        true },
		{ "Segfault",   "CRASH",    "Simon",              false },
		{ "Teapot",     "STEAM",    "Simon",              false },
		{ "Simon",      "FELL",     "out of the map",     false },
	};
	const int count = static_cast<int>(sizeof(kills) / sizeof(kills[0]));

	rng_ = rng_ * 1664525u + 1013904223u;
	const Kill& k = kills[(rng_ >> 8) % count];
	killfeed(k.killer, k.weapon, k.victim);
	if (k.chuckScores) {
		++chuckScore_;
	}
	else {
		++bugsScore_;
	}

	rng_ = rng_ * 1664525u + 1013904223u;
	nextFakeKill_ = now + 8.0f + static_cast<float>((rng_ >> 8) % 600) / 100.0f;
}

void ChuckEdition::unlock(Achievement achievement) {
	const std::size_t index = static_cast<std::size_t>(achievement);
	if (index >= unlocked_.size() || unlocked_[index]) {
		return;
	}
	unlocked_[index] = true;

	const AchievementInfo& info = Notifications::info(achievement);
	notifications_.achievement(achievement, time());
	std::cout << "  [ACHIEVEMENT] " << info.title << " (+" << info.xp << " XP)\n";
	awardXp(info.xp);
}

void ChuckEdition::callout(const std::string& text, const std::string& subtitle) {
	notifications_.callout(text, subtitle, time());
}

void ChuckEdition::killfeed(const std::string& killer, const std::string& weapon, const std::string& victim) {
	notifications_.killfeed(killer, weapon, victim, time());
}

void ChuckEdition::awardXp(int amount) {
	xp_ += amount;
	const int newLevel = levelForXp(xp_);
	if (newLevel > level_) {
		level_ = newLevel;
		callout("LEVEL UP", "LVL " + std::to_string(level_) + "  //  " + rankForLevel(level_));
		killfeed("Chuck", "LEVEL UP", "LVL " + std::to_string(level_) + " " + rankForLevel(level_));
	}
}

// Level L needs 50 * L * (L - 1) total XP: 0, 100, 300, 600, 1000, 1500, ...
// (each level costs 100 more than the last, the classic "just one more level" curve)
int ChuckEdition::xpForLevel(int level) {
	return 50 * level * (level - 1);
}

int ChuckEdition::levelForXp(int xp) {
	int level = 1;
	while (xpForLevel(level + 1) <= xp) {
		++level;
	}
	return level;
}

const char* ChuckEdition::rankForLevel(int level) {
	if (level >= 16) return "GRAND CHAMPION";
	if (level >= 13) return "CHAMPION";
	if (level >= 10) return "DIAMOND";
	if (level >= 7)  return "PLATINUM";
	if (level >= 5)  return "GOLD";
	if (level >= 3)  return "SILVER";
	return "BRONZE";
}

void ChuckEdition::cycleTheme() {
	// Start the crossfade FROM whatever is on screen right now (even mid-fade),
	// so mashing F2 never causes a visible jump. (F2 also drops you out of CHUCK MODE.)
	fromTheme_ = palette_;
	chuckMode_ = false;
	themeIndex_ = (themeIndex_ + 1) % GamerTheme::themeCount();
	themeSwitchTime_ = time();

	callout(GamerTheme::theme(themeIndex_).name, "RGB THEME");
	themesSeen_ |= 1u << themeIndex_;
	if (themesSeen_ == (1u << GamerTheme::themeCount()) - 1u) {
		unlock(Achievement::TasteTheRainbow);
	}

	std::cout << "  [RGB] Theme -> " << GamerTheme::theme(themeIndex_).name << "\n";
}

// ============================================================================ per-frame

float ChuckEdition::time() const {
	const std::chrono::duration<float> elapsed = std::chrono::steady_clock::now() - bootTime_;
	return elapsed.count();
}

void ChuckEdition::beginFrame() {
	const float now = time();

	// ---- frame timing
	frameMs_ = (now - lastFrameTime_) * 1000.0f;
	lastFrameTime_ = now;

	frameHistory_[frameHistoryHead_] = frameMs_;
	frameHistoryHead_ = (frameHistoryHead_ + 1) % frameHistorySize;

	++framesInWindow_;
	if (now - fpsWindowStart_ >= fpsWindowSeconds) {
		fps_ = framesInWindow_ / (now - fpsWindowStart_);
		framesInWindow_ = 0;
		fpsWindowStart_ = now;

		// "1% low" = the FPS you'd get if every frame were as slow as the slowest 1%.
		// With 120 samples that's the 2nd-worst frame. Pros quote this one, not the average.
		std::array<float, frameHistorySize> sorted = frameHistory_;
		std::nth_element(sorted.begin(), sorted.begin() + 1, sorted.end(), [](float a, float b) { return a > b; });
		onePercentLow_ = sorted[1] > 0.0f ? 1000.0f / sorted[1] : 0.0f;
	}

	// ---- ease the cinematic letterbox bars in and out
	const float frameDt = std::min(frameMs_ / 1000.0f, 0.1f);
	letterbox_ += ((cinematic_ ? 1.0f : 0.0f) - letterbox_) * std::min(1.0f, frameDt * 4.0f);
	helpAlpha_ += ((helpVisible_ ? 1.0f : 0.0f) - helpAlpha_) * std::min(1.0f, frameDt * 10.0f);

	// ---- resolve this frame's palette: crossfade between themes, then apply rainbow
	const float fade = smoothstep01((now - themeSwitchTime_) / themeFadeSeconds);
	const GamerTheme::Theme& target = chuckMode_ ? GamerTheme::chuckModeTheme() : GamerTheme::theme(themeIndex_);
	const GamerTheme::Theme blended = GamerTheme::blend(fromTheme_, target, fade);
	palette_ = GamerTheme::resolve(blended, now);

	// ---- the window title doubles as a live FPS readout (every pro streamer needs one)
	if (now - lastTitleUpdate_ >= fpsWindowSeconds) {
		lastTitleUpdate_ = now;
		updateWindowTitle(Engine::getInstance().getCurrentScene());
	}
}

void ChuckEdition::updateWindowTitle(const Scene* scene) {
	std::ostringstream title;
	title << "SimonEngine2 :: CHUCK SPECIAL EDITION";
	if (scene) {
		title << "  |  " << static_cast<int>(std::lround(fps_)) << " FPS  |  " << mapName(*scene);
	}
	title << "  |  " << homeworkSignature;
	glutSetWindowTitle(title.str().c_str());
}

void ChuckEdition::drawBackdrop() {
	if (sceneryEnabled_) {
		Backdrop::draw(palette_, time(), horizonFraction_);
	}
}

void ChuckEdition::beginWorld(const Scene& scene) {
	// Everything the world pass changes gets rolled back in endWorld(), so the
	// next thing drawn (the HUD) starts from exactly the state the engine expects
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// Grab the camera matrices so the HUD can pin labels to 3D positions later
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_);
	glGetDoublev(GL_PROJECTION_MATRIX, projection_);
	glGetIntegerv(GL_VIEWPORT, viewport_);
	haveMatrices_ = true;

	// In perspective, the real horizon is where a point infinitely far away, at camera
	// height, lands on screen. Any point at EXACTLY camera height projects onto that same
	// line no matter how far out it is, so probe 50 units ahead (inside the far plane, or
	// worldToScreen rejects it) and park the 2D backdrop's horizon (sun + skyline) there.
	// Now the 3D floor and the 2D sky actually meet.
	// Ortho cameras have no horizon at all, so they keep the default.
	const Camera& camera = scene.getCamera();
	horizonFraction_ = 0.60f;
	if (camera.getProjectionMode() == Camera::ProjectionMode::Perspective) {
		EngineMath::Vector3 flat = camera.getTarget() - camera.getPosition();
		flat.y_ = 0.0f;
		if (flat.lengthSquared() > 1e-6f) {
			const EngineMath::Vector3 distantPoint = camera.getPosition() + flat.normalized() * 50.0f;
			float sx = 0.0f;
			float sy = 0.0f;
			if (worldToScreen(distantPoint, sx, sy) && viewport_[3] > 0) {
				horizonFraction_ = std::min(std::max(sy / viewport_[3], 0.05f), 0.95f);
			}
		}
	}

	if (lightingEnabled_) {
		StudioLighting::apply(palette_);
	}
	else {
		StudioLighting::disable();
	}
}

void ChuckEdition::endWorld(const Scene& scene) {
	if (sceneryEnabled_) {
		float floorY = 0.0f;
		float padRadius = 1.0f;
		float extent = 8.0f;
		float spacing = 0.5f;
		floorFor(scene, floorY, padRadius, extent, spacing);
		NeonWorld::drawFloor(palette_, time(), floorY, padRadius, extent, spacing);
	}

	// Map-specific world extras (Chuck World's blob shadows and core light pools)
	if (const ChuckWorldScene* world = dynamic_cast<const ChuckWorldScene*>(&scene)) {
		world->drawWorldExtras();
	}

	glPopAttrib();
}

int ChuckEdition::outlinePassCount() const {
	return outlinesEnabled_ ? NeonWorld::outlinePassCount() : 0;
}

void ChuckEdition::beginOutlinePass(int pass) {
	NeonWorld::beginOutlinePass(palette_, pass);
}

void ChuckEdition::endOutlinePass() {
	NeonWorld::endOutlinePass();
}

void ChuckEdition::drawOverlay(const Scene& scene) {
	Render2D::begin();

	const GamerTheme::Theme& p = palette_;
	const float w = static_cast<float>(Render2D::width());
	const float h = static_cast<float>(Render2D::height());

	// A handful of big blurry embers floating IN FRONT of the robot = instant depth
	if (sceneryEnabled_) {
		Backdrop::drawForegroundEmbers(p, time());
	}

	// Subtle lens vignette on the world (before the HUD so the HUD stays crisp)
	PostFX::vignette(0.38f);

	// Photo mode and cinematic mode both put the HUD away
	const bool showHud = !photoMode_ && !cinematic_;

	// ---- the HUD: fill in this frame's numbers, let Hud draw them
	Hud::HudInfo info;
	info.time = time();
	info.reveal = hudReveal();
	info.fps = fps_;
	info.frameMs = frameMs_;
	info.onePercentLowFps = onePercentLow_;
	info.frameHistoryMs = frameHistory_.data();
	info.frameHistorySize = frameHistorySize;
	info.frameHistoryHead = frameHistoryHead_;
	info.mapName = mapName(scene);
	info.themeName = p.name;
	info.cameraMode = scene.getCamera().getProjectionMode() == Camera::ProjectionMode::Perspective ? "PERSP" : "ORTHO";
	info.cameraZoom = static_cast<float>(scene.getCamera().getZoom());
	info.objectCount = static_cast<int>(scene.countObjects());
	info.sessionSeconds = time();
	info.rayTracing = lightingEnabled_;
	info.showStats = statsVisible_;
	info.level = level_;
	info.rank = chuckMode_ ? "CHUCK (MAX RANK)" : rankForLevel(level_);
	info.xp = xp_;
	info.xpLevelStart = xpForLevel(level_);
	info.xpLevelEnd = xpForLevel(level_ + 1);
	info.chuckScore = chuckScore_;
	info.bugsScore = bugsScore_;
	info.isGameMap = dynamic_cast<const ChuckWorldScene*>(&scene) != nullptr;
	if (showHud) {
		Hud::draw(info, p);

		// Chuck World brings its own game HUD (objective, score, nametag, popups...)
		if (const ChuckWorldScene* world = dynamic_cast<const ChuckWorldScene*>(&scene)) {
			world->drawHud(p, time());
		}
	}

	// Cinematic black bars, with the watermark tucked into the bottom one like a trailer
	PostFX::letterbox(letterbox_);

	if (!photoMode_) {
		// Watermark, bottom-right. Every legit special edition needs one.
		const std::string tag = "CHUCK SPECIAL EDITION";
		const float tagW = Render2D::strokeTextWidth(tag, 11.0f, 2.0f);
		Render2D::strokeTextNeon(w - tagW - 14.0f, h - 14.0f, tag, 11.0f, p.accent, 0.85f, 2.0f);

		if (letterbox_ > 0.5f) {
			Render2D::text(16.0f, h - 14.0f, "CINEMATIC MODE  //  " + mapName(scene) + "  //  F5 TO EXIT", Render2D::Font::Tiny, p.accent2, (letterbox_ - 0.5f) * 2.0f);
		}

		// Achievement toasts, callouts and the killfeed sit above the HUD
		notifications_.draw(p, time(), 136.0f);
	}

	// Konami confetti rains over everything except the CRT glass
	Hud::drawConfetti(time() - confettiStart_);

	// F1 controls sheet
	if (!photoMode_) {
		Hud::drawControls(p, time(), helpAlpha_);
	}

	// F8: the whole frame goes through a 1998 CRT, HUD included (that's how CRTs work)
	if (crtEnabled_) {
		PostFX::crt(p, time());
	}

	// The TAB loading wipe goes over EVERYTHING
	drawMapTransition(w, h);

	// ...except the boot splash, which fades out to reveal the game underneath
	const float splashAlpha = splashEndTime_ < 0.0f ? 1.0f : 1.0f - (time() - splashEndTime_) / SplashScreen::fadeSeconds;
	if (splashAlpha > 0.0f) {
		SplashScreen::draw(p, time(), splashAlpha);
	}

	// F9: grab the finished frame from the back buffer BEFORE the flash gets drawn on it
	if (screenshotRequested_) {
		takeScreenshot();
	}
	PostFX::flash(0.85f * (1.0f - std::min(1.0f, (time() - flashStart_) / 0.35f)));

	Render2D::end();
}

// ============================================================================ helpers

std::string ChuckEdition::mapName(const Scene& scene) {
	if (dynamic_cast<const RobotScene*>(&scene)) {
		return "de_robot";
	}
	if (dynamic_cast<const ObjScene*>(&scene)) {
		return "de_teapot";
	}
	if (dynamic_cast<const MagicCubeScene*>(&scene)) {
		return "de_magiccube";
	}
	if (dynamic_cast<const ChuckWorldScene*>(&scene)) {
		return "de_chuckworld";
	}
	return "de_unknown";
}

bool ChuckEdition::worldToScreen(const EngineMath::Vector3& world, float& x, float& y) const {
	if (!haveMatrices_) {
		return false;
	}

	GLdouble wx = 0.0;
	GLdouble wy = 0.0;
	GLdouble wz = 0.0;
	if (gluProject(world.x_, world.y_, world.z_, modelview_, projection_, viewport_, &wx, &wy, &wz) != GL_TRUE) {
		return false;
	}

	// Window depth outside [0, 1] = behind the camera (or past the far plane)
	if (wz < 0.0 || wz > 1.0) {
		return false;
	}

	// gluProject gives window coordinates with y pointing UP; the HUD's y points DOWN
	x = static_cast<float>(wx);
	y = static_cast<float>(viewport_[3] - wy);
	return true;
}

// ============================================================================ map rotation

int ChuckEdition::mapIndex(const Scene& scene) {
	if (dynamic_cast<const ChuckWorldScene*>(&scene)) {
		return 1;
	}
	if (dynamic_cast<const ObjScene*>(&scene)) {
		return 2;
	}
	if (dynamic_cast<const MagicCubeScene*>(&scene)) {
		return 3;
	}
	return 0; // the homework robot
}

const char* ChuckEdition::mapNameByIndex(int index) {
	static const char* const names[mapCount] = { "de_robot", "de_chuckworld", "de_teapot", "de_magiccube" };
	return names[((index % mapCount) + mapCount) % mapCount];
}

void ChuckEdition::startMapTransition(const Scene& scene) {
	transitionStart_ = time();
	transitionTarget_ = (mapIndex(scene) + 1) % mapCount;
	transitionSwapped_ = false;

	// The stats panels cover half the arena, so tuck them away for the game map (F3 brings them back)
	if (transitionTarget_ == 1) {
		statsVisible_ = false;
	}
	std::cout << "  [MAP] Loading " << mapNameByIndex(transitionTarget_) << "...\n";
}

void ChuckEdition::updateMapTransition() {
	if (transitionStart_ < 0.0f) {
		return;
	}

	const float t = time() - transitionStart_;

	// Swap maps only once the wipe fully covers the screen, so you never see the pop
	if (!transitionSwapped_ && t >= wipeCoverSeconds) {
		transitionSwapped_ = true;
		switch (transitionTarget_) {
			case 1:  Engine::getInstance().changeScene(std::make_unique<ChuckWorldScene>()); break;
			case 2:  Engine::getInstance().changeScene(std::make_unique<ObjScene>()); break;
			case 3:  Engine::getInstance().changeScene(std::make_unique<MagicCubeScene>()); break;
			default: Engine::getInstance().changeScene(std::make_unique<RobotScene>()); break;
		}
	}

	if (t >= wipeTotalSeconds) {
		transitionStart_ = -1.0f;
	}
}

void ChuckEdition::drawMapTransition(float w, float h) const {
	if (transitionStart_ < 0.0f) {
		return;
	}

	const float t = time() - transitionStart_;
	const float skew = h * 0.35f;
	const GamerTheme::Theme& p = palette_;

	// Leading and trailing edge of a slanted panel. Cover: the leading edge sweeps in from
	// the left. Uncover: the trailing edge chases it off to the right.
	float lead = 0.0f;
	float trail = 0.0f;
	if (t < wipeCoverSeconds) {
		lead = smoothstep01(t / wipeCoverSeconds);
		trail = 0.0f;
	}
	else {
		lead = 1.0f;
		trail = smoothstep01((t - wipeCoverSeconds) / (wipeTotalSeconds - wipeCoverSeconds));
	}

	const float span = w + skew * 2.0f;
	const float leadX = -skew + lead * span;
	const float trailX = -skew + trail * span;

	// The panel: a parallelogram leaning right
	const GamerTheme::Rgb panelColor = GamerTheme::scale(p.skyTop, 0.8f);
	glColor4f(panelColor.r, panelColor.g, panelColor.b, 1.0f);
	glBegin(GL_QUADS);
	glVertex2f(trailX - skew, 0.0f);
	glVertex2f(leadX + skew, 0.0f);
	glVertex2f(leadX - skew, h);
	glVertex2f(trailX - skew * 3.0f, h);
	glEnd();

	// A neon stripe riding each edge
	Render2D::blendAdditive();
	glColor4f(p.accent.r, p.accent.g, p.accent.b, 0.9f);
	glBegin(GL_QUADS);
	glVertex2f(leadX + skew, 0.0f);
	glVertex2f(leadX + skew + 10.0f, 0.0f);
	glVertex2f(leadX - skew + 10.0f, h);
	glVertex2f(leadX - skew, h);
	glEnd();
	glColor4f(p.accent2.r, p.accent2.g, p.accent2.b, 0.9f);
	glBegin(GL_QUADS);
	glVertex2f(trailX - skew - 6.0f, 0.0f);
	glVertex2f(trailX - skew, 0.0f);
	glVertex2f(trailX - skew * 3.0f, h);
	glVertex2f(trailX - skew * 3.0f - 6.0f, h);
	glEnd();
	Render2D::blendNormal();

	// While fully covered: LOADING de_whatever, with a spinner
	const float coverAlpha = (t > wipeCoverSeconds * 0.8f && trail < 0.25f) ? 1.0f - trail * 4.0f : 0.0f;
	if (coverAlpha > 0.0f) {
		const std::string loading = "LOADING";
		const std::string map = mapNameByIndex(transitionTarget_);
		const float lw = static_cast<float>(Render2D::textWidth(loading, Render2D::Font::Body));
		const float mw = Render2D::strokeTextWidth(map, 26.0f, 2.0f);
		Render2D::text((w - lw) * 0.5f, h * 0.5f - 30.0f, loading, Render2D::Font::Body, p.accent2, coverAlpha);
		Render2D::strokeTextNeon((w - mw) * 0.5f, h * 0.5f + 8.0f, map, 26.0f, p.accent, coverAlpha, 2.0f);

		for (int i = 0; i < 8; ++i) {
			const float a = time() * 8.0f + i * (2.0f * EngineMath::PI / 8.0f);
			Render2D::rect(w * 0.5f + std::cos(a) * 14.0f - 2.0f, h * 0.5f + 40.0f + std::sin(a) * 14.0f - 2.0f, 4.0f, 4.0f, p.accent, coverAlpha * (0.2f + 0.8f * i / 8.0f));
		}
	}
}

void ChuckEdition::floorFor(const Scene& scene, float& floorY, float& padRadius, float& extent, float& spacing) {
	extent = 8.0f;
	spacing = 0.5f;

	if (dynamic_cast<const ChuckWorldScene*>(&scene)) {
		// Chuck World: real floor at 0, a big arena, chunkier grid, spawn pad at the origin
		floorY = 0.0f;
		padRadius = 1.0f;
		extent = ChuckWorldScene::floorExtent;
		spacing = ChuckWorldScene::floorSpacing;
		return;
	}

	// Hand-measured so every model stands exactly ON the neon floor:
	//   robot:  legs are 1.5 tall, hanging below a 1.5 tall torso centered on 0 -> -2.25
	//   teapot: the .obj's lowest vertex is y = 0
	//   cube:   spins, so give it a generous gap underneath
	if (dynamic_cast<const ObjScene*>(&scene)) {
		floorY = -0.01f;
		padRadius = 1.05f;
	}
	else if (dynamic_cast<const MagicCubeScene*>(&scene)) {
		floorY = -2.0f;
		padRadius = 1.35f;
	}
	else {
		floorY = -2.26f;
		padRadius = 1.15f;
	}
}
