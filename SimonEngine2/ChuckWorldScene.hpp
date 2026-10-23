#ifndef CHUCK_WORLD_SCENE_HPP
#define CHUCK_WORLD_SCENE_HPP

/*
 *    ___ _  _ _   _  ___ _  __ __      _____  ___ _    ___
 *   / __| || | | | |/ __| |/ / \ \    / / _ \| _ \ |  |   \     SimonEngine2 :: CHUCK SPECIAL EDITION
 *  | (__| __ | |_| | (__| ' <   \ \/\/ / (_) |   / |__| |) |    map: de_chuckworld
 *   \___|_||_|\___/ \___|_|\_\   \_/\_/ \___/|_|_\____|___/     "a world to walk in"
 *
 *  A tiny but complete playable game built on top of SimonEngine2:
 *
 *    - Walk ChuckBot around a neon arena (WASD / arrows, camera-relative)
 *    - Jump (SPACE) onto crates and towers, real gravity, step-up, landing
 *    - Third-person follow camera: Q/E or right-drag to orbit, scroll to zoom
 *    - Collision against crates (circle vs box), pillars (circle vs circle) and the
 *      arena wall
 *    - 10 power cores per round, some on the floor, some only reachable by
 *      platforming. Grab them fast to chain a COMBO multiplier.
 *    - Round timer, best time, score, VICTORY ROYALE screen, auto next round
 *    - Floating "+100" popups, a player nametag, and a waypoint to the nearest core
 *
 *  Physics/gameplay lives here in the scene; ChuckBot and PowerCore are just animated
 *  Objects. The 2D game HUD is drawn by drawHud(), which the Chuck Edition overlay calls.
 */

#include <memory>
#include <string>
#include <vector>

#include "Scene.hpp"
#include "GamerTheme.hpp"
#include "ChuckBot.hpp"
#include "PowerCore.hpp"

class ChuckWorldScene : public Scene {
	public:
		ChuckWorldScene() : Scene(244, 244, 10) {}

		~ChuckWorldScene() {}

		virtual void init() override final;

		virtual void draw() const override final {}

		virtual void update() override final;

		virtual void deInit() override final;

		// ---- hooks for the Chuck Edition overlay
		void drawWorldExtras() const;                                     // 3D: blob shadows (after objects)
		void drawHud(const GamerTheme::Theme& palette, float time) const; // 2D: game HUD (inside Render2D)

		// The arena is big, so the neon floor needs to be too
		static constexpr float floorExtent = 22.0f;
		static constexpr float floorSpacing = 1.0f;

	private:
		// Axis-aligned box obstacle you can collide with or stand on
		struct Crate {
			float x, z;     // center on the floor
			float hx, hz;   // half extents
			float top;      // height of the top face
		};

		// Round obstacle (pillars, the arena wall posts)
		struct Pillar {
			float x, z, radius, height;
		};

		struct Popup {
			EngineMath::Vector3 position;
			float born;
			std::string text;
		};

		void buildArena();
		void addCrate(float x, float z, float size, float height, const EngineUtil::ColorRGB& color);
		void addPillar(float x, float z, float radius, float height, const EngineUtil::ColorRGB& color, const EngineUtil::ColorRGB& capColor);

		void startRound();
		void updatePlayer(float dt);
		void updateCamera(float dt);
		void updatePickups();
		float groundHeightAt(float x, float z, float feetY) const;
		bool cameraBlocked(const EngineMath::Vector3& position) const;

		ChuckBot* player_ = nullptr;
		std::vector<PowerCore*> cores_;
		std::vector<Crate> crates_;
		std::vector<Pillar> pillars_;
		std::vector<EngineMath::Vector3> coreSpots_;
		std::vector<Popup> popups_;

		// ---- player physics state
		EngineMath::Vector3 playerPos_ = { 0.0f, 0.0f, 0.0f };
		EngineMath::Vector3 velocity_ = { 0.0f, 0.0f, 0.0f };
		bool grounded_ = true;
		float facing_ = 0.0f;
		float shadowY_ = 0.0f;

		// ---- camera rig
		float cameraYaw_ = 0.0f;
		float cameraPitch_ = 0.24f;    // low-ish, so the sunset stays in frame
		float cameraDistance_ = 7.5f;   // where the player WANTS the camera (scroll wheel)
		float cameraBoom_ = 7.5f;       // where it actually is after dodging walls
		EngineMath::Vector3 cameraFocus_ = { 0.0f, 1.1f, 0.0f };

		// ---- round state
		int round_ = 0;
		int score_ = 0;
		int combo_ = 0;
		float lastPickupTime_ = -100.0f;
		float roundStart_ = 0.0f;
		float roundTime_ = 0.0f;
		float bestTime_ = 0.0f;
		bool victory_ = false;
		bool newBest_ = false;
		float victoryTime_ = 0.0f;

		float lastTime_ = -1.0f;
		unsigned int rng_ = 0xC0FFEEu;
};

#endif
