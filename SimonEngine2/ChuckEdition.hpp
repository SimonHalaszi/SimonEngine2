#ifndef CHUCK_EDITION_HPP
#define CHUCK_EDITION_HPP

/*
 *    ___ _  _ _   _  ___ _  __  ___ ___ ___ _____ ___ ___  _  _
 *   / __| || | | | |/ __| |/ / | __|   \_ _|_   _|_ _/ _ \| \| |
 *  | (__| __ | |_| | (__| ' <  | _|| |) | |  | |  | | (_) | .` |
 *   \___|_||_|\___/ \___|_|\_\ |___|___/___| |_| |___\___/|_|\_|
 *
 *  SimonEngine2, but better. The CHUCK SPECIAL EDITION mission control.
 *
 *  This singleton is the one place every piece of Chuck Edition eye candy hangs off of.
 *  The original engine only needed a few one-line hooks to support it:
 *
 *     Engine::init()          -> init()          once the window + GL context exist
 *     Engine::updateTimer()   -> update(scene)   every tick, BEFORE the scene updates
 *     GAMEdraw()              -> beginFrame()    top of the display callback
 *                             -> drawBackdrop()  right after glClear, before the 3D scene
 *     Scene::sceneDraw()      -> beginWorld()    right after the camera is applied
 *                             -> endWorld()      after the scene's objects are drawn
 *     GAMEdraw()              -> drawOverlay()   after the scene: HUD, toasts, post FX
 *
 *  The homework logic (scenes, objects, input) is untouched; Chuck Edition only *reads*
 *  input to react to it, and only *consumes* it while the splash screen is up.
 */

#include <array>
#include <chrono>
#include <string>

#include "GamerTheme.hpp"
#include "EngineMath.hpp"
#include "Notifications.hpp"

class Scene;

class ChuckEdition {
	public:
		static ChuckEdition& getInstance() {
			static ChuckEdition instance;
			return instance;
		}

		// ------------------------------------------------------------ engine hooks
		void init();
		void update(Scene* scene);
		void beginFrame();
		void drawBackdrop();
		void beginWorld(const Scene& scene);
		void endWorld(const Scene& scene);
		void drawOverlay(const Scene& scene);

		// Neon outline passes: Scene::sceneDraw() redraws its objects once per pass
		int outlinePassCount() const;
		void beginOutlinePass(int pass);
		void endOutlinePass();

		// ------------------------------------------------------------ shared state
		float time() const; // seconds since boot, high resolution
		const GamerTheme::Theme& palette() const { return palette_; } // resolved for this frame

		float fps() const { return fps_; }
		float frameMs() const { return frameMs_; }

		bool isRayTracingOn() const { return lightingEnabled_; } // (it's studio lighting, shh)

		// "de_robot", "de_teapot", ... (Counter-Strike map naming, obviously)
		static std::string mapName(const Scene& scene);

		// Projects a world-space point to HUD pixels (y down) using the camera of the frame
		// being drawn. Returns false if the point is behind the camera.
		bool worldToScreen(const EngineMath::Vector3& world, float& x, float& y) const;

		// ------------------------------------------------------------ hype API
		// Anything (scenes included) can fire these. Unlocking twice is a no-op.
		void unlock(Achievement achievement);
		void callout(const std::string& text, const std::string& subtitle = "");
		void killfeed(const std::string& killer, const std::string& weapon, const std::string& victim);
		void awardXp(int amount);

		ChuckEdition(const ChuckEdition&) = delete;
		ChuckEdition& operator=(const ChuckEdition&) = delete;
		ChuckEdition(const ChuckEdition&&) = delete;
		ChuckEdition& operator=(const ChuckEdition&&) = delete;

	private:
		ChuckEdition() : bootTime_(std::chrono::steady_clock::now()) {}
		~ChuckEdition() {}

		void printConsoleBanner() const;
		static void floorFor(const Scene& scene, float& floorY, float& padRadius, float& extent, float& spacing);

		// ---- map rotation (TAB)
		static int mapIndex(const Scene& scene);
		static const char* mapNameByIndex(int index);
		void startMapTransition(const Scene& scene);
		void updateMapTransition();
		void drawMapTransition(float w, float h) const;

		// ---- camera toys for the homework maps (Chuck World drives its own camera)
		void updateCameraControls(Scene* scene, float dt);
		void takeScreenshot();

		// ---- splash
		bool updateSplash();          // true while the splash still owns the input
		static bool anyInputPressed();
		float hudReveal() const;      // 0..1, panels slide in after the splash

		// ---- secrets
		void updateSecrets();
		void advanceKonami(int input);
		void activateChuckMode();
		void checkTypedWord();

		// ---- progression / notifications
		void observeHomeworkInput(const Scene* scene);
		void updateFakeKillfeed();
		static int levelForXp(int xp);
		static int xpForLevel(int level);
		static const char* rankForLevel(int level);
		void cycleTheme();
		void updateWindowTitle(const Scene* scene);

		std::chrono::steady_clock::time_point bootTime_;

		// ---- theme crossfade
		int themeIndex_ = 0;
		GamerTheme::Theme fromTheme_ = GamerTheme::theme(0);
		float themeSwitchTime_ = -100.0f;
		GamerTheme::Theme palette_ = GamerTheme::theme(0);

		// ---- feature toggles
		bool lightingEnabled_ = true; // F7
		bool sceneryEnabled_ = true;  // F10 (backdrop sky / sun / skyline / embers / floor)
		bool outlinesEnabled_ = true; // F6
		bool statsVisible_ = true;    // F3
		bool cinematic_ = false;      // F5 (auto-orbit + letterbox, HUD tucked away)
		bool crtEnabled_ = false;     // F8
		bool photoMode_ = false;      // F12 (every overlay hidden, pure scenery)
		bool helpVisible_ = false;    // F1
		float helpAlpha_ = 0.0f;      // animated 0..1
		bool screenshotRequested_ = false; // F9, captured at the end of the next overlay pass
		float letterbox_ = 0.0f;      // animated 0..1
		float flashStart_ = -100.0f;  // screenshot flash
		float lastUpdateTime_ = -1.0f;

		// ---- secrets
		bool chuckMode_ = false;      // the Konami code reward
		int konamiProgress_ = 0;
		float confettiStart_ = -100.0f;
		std::string typedHistory_;    // last few typed characters, for the word easter eggs

		// ---- splash screen (negative = still showing)
		float splashEndTime_ = -1.0f;

		// ---- progression + notifications
		Notifications notifications_;
		std::array<bool, static_cast<std::size_t>(Achievement::Count)> unlocked_ = {};
		int xp_ = 0;
		int level_ = 1;
		float lastPassiveXp_ = 0.0f;
		bool sawAnyInput_ = false;
		int elbowBends_ = 0;
		bool grayscale_ = false;
		unsigned int themesSeen_ = 1u;  // bit per theme index (we boot into theme 0)
		unsigned int mapsSeen_ = 0u;    // bit per map index
		int chuckScore_ = 0;            // the esports scoreboard: CHUCK vs BUGS
		int bugsScore_ = 0;
		float nextFakeKill_ = 6.0f;
		unsigned int rng_ = 0x5EED5u;

		// ---- TAB map transition (negative start = not transitioning)
		float transitionStart_ = -1.0f;
		int transitionTarget_ = 0;
		bool transitionSwapped_ = false;

		// ---- camera matrices captured in beginWorld() for worldToScreen()
		double modelview_[16] = {};
		double projection_[16] = {};
		int viewport_[4] = {};
		bool haveMatrices_ = false;
		float horizonFraction_ = 0.60f; // fed to the backdrop next frame

		// ---- frame timing (real numbers, measured with a steady clock)
		float lastFrameTime_ = 0.0f;
		float fpsWindowStart_ = 0.0f;
		int framesInWindow_ = 0;
		float fps_ = 0.0f;
		float frameMs_ = 0.0f;
		float onePercentLow_ = 0.0f;
		float lastTitleUpdate_ = -1.0f;

		// Ring buffer of recent frame times for the HUD graph. head_ = oldest sample.
		static constexpr int frameHistorySize = 120;
		std::array<float, frameHistorySize> frameHistory_ = {};
		int frameHistoryHead_ = 0;
		std::string lastMapName_;
};

#endif
