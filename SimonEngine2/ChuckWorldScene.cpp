#include "ChuckWorldScene.hpp"

#include <GL/freeglut.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "Engine.hpp"
#include "InputManager.hpp"
#include "RectangularPrism.hpp"
#include "Sphere.hpp"
#include "Cylinder.hpp"
#include "Render2D.hpp"
#include "ChuckEdition.hpp"

namespace {
	// ---------------------------------------------------------------- tuning knobs
	constexpr float arenaRadius = 13.4f;     // how far from the center you can walk
	constexpr float wallRadius = 14.6f;      // where the wall posts stand
	constexpr float playerRadius = 0.35f;
	constexpr float moveSpeed = 5.2f;        // units per second
	constexpr float acceleration = 12.0f;    // how snappy speeding up / stopping feels
	constexpr float jumpVelocity = 7.2f;     // apex ~1.3 units: clears a crate, not a tower
	constexpr float gravity = 20.0f;
	constexpr float stepUp = 0.3f;           // auto-climb ledges up to this tall when landing
	constexpr float pickupRadius = 0.95f;
	constexpr float comboWindow = 3.0f;      // seconds between pickups to keep a combo alive
	constexpr int coresPerRound = 10;
	constexpr float victoryHold = 4.5f;      // seconds of VICTORY ROYALE before the next round

	const EngineUtil::ColorRGB crateNavy(35, 42, 85);
	const EngineUtil::ColorRGB towerPurple(60, 36, 105);
	const EngineUtil::ColorRGB pillarDark(28, 28, 52);
	const EngineUtil::ColorRGB capMagenta(255, 60, 200);
	const EngineUtil::ColorRGB capCyan(40, 220, 255);

	float clampf(float v, float lo, float hi) {
		return std::min(std::max(v, lo), hi);
	}

	// Shortest signed angle from a to b, so turning from 350 to 10 degrees goes the
	// short way (+20) instead of spinning all the way around (-340)
	float angleDelta(float from, float to) {
		float d = std::fmod(to - from + EngineMath::PI, 2.0f * EngineMath::PI);
		if (d < 0.0f) {
			d += 2.0f * EngineMath::PI;
		}
		return d - EngineMath::PI;
	}

	std::string formatTime(float seconds) {
		const int whole = static_cast<int>(seconds);
		const int tenths = static_cast<int>((seconds - whole) * 10.0f);
		std::ostringstream out;
		out << std::setfill('0') << std::setw(2) << whole / 60 << ":" << std::setw(2) << whole % 60 << "." << tenths;
		return out.str();
	}

	bool keyDown(char lower) {
		const InputManager& in = InputManager::getInstance();
		return in.isDown(static_cast<unsigned char>(lower)) || in.isDown(static_cast<unsigned char>(lower - 'a' + 'A'));
	}

	bool keyPressed(char lower) {
		const InputManager& in = InputManager::getInstance();
		return in.isPressed(static_cast<unsigned char>(lower)) || in.isPressed(static_cast<unsigned char>(lower - 'a' + 'A'));
	}
}

// ============================================================================ setup

void ChuckWorldScene::init() {
	buildArena();

	std::unique_ptr<ChuckBot> bot = std::make_unique<ChuckBot>(EngineMath::Vector3(0.0f, 0.0f, 0.0f));
	player_ = bot.get();
	addRootObject(std::move(bot));

	for (int i = 0; i < coresPerRound; ++i) {
		std::unique_ptr<PowerCore> core = std::make_unique<PowerCore>(EngineMath::Vector3(0.0f, -50.0f, 0.0f));
		cores_.push_back(core.get());
		addRootObject(std::move(core));
	}

	// Real perspective for the third-person camera (the homework scenes default to ortho)
	camera_.setPerspective(62.0, 0.1, 150.0);

	cameraFocus_ = EngineMath::Vector3(0.0f, 1.1f, 0.0f);
	updateCamera(0.0f);

	startRound();
}

void ChuckWorldScene::buildArena() {
	// ---------------------------------------------------------------- the wall
	// 20 posts in a ring with glowing caps, alternating magenta / cyan
	const int posts = 20;
	for (int i = 0; i < posts; ++i) {
		const float a = 2.0f * EngineMath::PI * i / posts;
		addPillar(std::cos(a) * wallRadius, std::sin(a) * wallRadius, 0.45f, 2.6f, pillarDark, (i % 2) ? capMagenta : capCyan);
	}

	// ---------------------------------------------------------------- inner pillars
	addPillar(0.0f, 7.5f, 0.6f, 3.2f, pillarDark, capMagenta);
	addPillar(-9.0f, 6.0f, 0.6f, 3.2f, pillarDark, capCyan);
	addPillar(9.5f, 7.0f, 0.6f, 3.2f, pillarDark, capMagenta);
	addPillar(7.0f, -8.5f, 0.6f, 3.2f, pillarDark, capCyan);

	// ---------------------------------------------------------------- crates + towers
	// Two little staircases (crate -> tower -> taller tower) make the platforming route
	addCrate(4.0f, 2.0f, 1.2f, 1.0f, crateNavy);
	addCrate(4.0f, 3.45f, 1.4f, 2.0f, towerPurple);   // step up from the crate

	addCrate(-6.0f, -4.6f, 1.2f, 1.0f, crateNavy);
	addCrate(-6.0f, -6.05f, 1.4f, 2.0f, towerPurple);
	addCrate(-7.6f, -6.05f, 1.4f, 3.0f, towerPurple); // the boss tower

	// Scattered single crates for cover and parkour
	addCrate(-4.0f, 3.0f, 1.2f, 1.0f, crateNavy);
	addCrate(2.0f, -6.0f, 1.2f, 1.0f, crateNavy);
	addCrate(8.5f, -2.0f, 1.2f, 1.0f, crateNavy);
	addCrate(-9.5f, 0.5f, 1.2f, 1.0f, crateNavy);
	addCrate(-2.5f, 9.5f, 1.2f, 1.0f, crateNavy);

	// ---------------------------------------------------------------- where cores can spawn
	// (y = the surface it sits on + 0.75)
	const float g = 0.75f;
	coreSpots_ = {
		// on the floor
		{ 0.0f, g, -4.0f }, { 6.5f, g, 6.0f }, { -5.0f, g, 0.5f }, { -11.0f, g, -3.0f },
		{ 11.0f, g, 2.0f }, { 0.0f, g, 11.0f }, { 3.5f, g, -10.5f }, { -5.5f, g, 9.0f },
		{ 10.5f, g, -6.5f }, { -2.0f, g, -10.0f },
		// on crates (jump!)
		{ 4.0f, 1.0f + g, 2.0f }, { -4.0f, 1.0f + g, 3.0f }, { 2.0f, 1.0f + g, -6.0f },
		{ 8.5f, 1.0f + g, -2.0f }, { -9.5f, 1.0f + g, 0.5f }, { -2.5f, 1.0f + g, 9.5f },
		// on towers (parkour required)
		{ 4.0f, 2.0f + g, 3.45f }, { -6.0f, 2.0f + g, -6.05f }, { -7.6f, 3.0f + g, -6.05f }
	};
}

void ChuckWorldScene::addCrate(float x, float z, float size, float height, const EngineUtil::ColorRGB& color) {
	addRootObject(std::make_unique<RectangularPrism>(
		EngineMath::Transform(
			EngineMath::Vector3(x, height * 0.5f, z),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(size, height, size)
		),
		color
	));

	// A thin glowing strip around the top edge so you can read heights at a glance
	addRootObject(std::make_unique<RectangularPrism>(
		EngineMath::Transform(
			EngineMath::Vector3(x, height - 0.04f, z),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(size + 0.04f, 0.06f, size + 0.04f)
		),
		height > 1.5f ? capMagenta : capCyan
	));

	crates_.push_back(Crate{ x, z, size * 0.5f, size * 0.5f, height });
}

void ChuckWorldScene::addPillar(float x, float z, float radius, float height, const EngineUtil::ColorRGB& color, const EngineUtil::ColorRGB& capColor) {
	addRootObject(std::make_unique<Cylinder>(
		EngineMath::Transform(
			EngineMath::Vector3(x, height * 0.5f, z),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(radius * 2.0f, height, radius * 2.0f)
		),
		color,
		16
	));
	addRootObject(std::make_unique<Sphere>(
		EngineMath::Transform(
			EngineMath::Vector3(x, height + radius * 0.4f, z),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(radius * 1.4f, radius * 1.4f, radius * 1.4f)
		),
		capColor,
		12,
		8
	));

	pillars_.push_back(Pillar{ x, z, radius, height + radius * 1.1f });
}

void ChuckWorldScene::startRound() {
	++round_;
	victory_ = false;
	newBest_ = false;
	combo_ = 0;
	roundStart_ = ChuckEdition::getInstance().time();
	roundTime_ = 0.0f;

	// Shuffle the spawn spots (Fisher-Yates with a tiny LCG) and hand the first 10 out.
	// Always force at least one tower spot into the round so you have to do some parkour.
	std::vector<EngineMath::Vector3> spots = coreSpots_;
	for (std::size_t i = spots.size() - 1; i > 0; --i) {
		rng_ = rng_ * 1664525u + 1013904223u;
		std::swap(spots[i], spots[(rng_ >> 8) % (i + 1)]);
	}
	bool hasTower = false;
	for (int i = 0; i < coresPerRound; ++i) {
		hasTower = hasTower || spots[i].y_ > 2.0f;
	}
	if (!hasTower) {
		spots[0] = coreSpots_.back();
	}

	for (int i = 0; i < coresPerRound && i < static_cast<int>(cores_.size()); ++i) {
		cores_[i]->respawn(spots[i]);
	}
}

// ============================================================================ update

void ChuckWorldScene::update() {
	InputManager& input = InputManager::getInstance();

	if (input.isPressed(27)) {
		Engine::getInstance().safeExit();
	}

	const float now = ChuckEdition::getInstance().time();
	const float dt = lastTime_ < 0.0f ? 0.0f : clampf(now - lastTime_, 0.0f, 1.0f / 30.0f);
	lastTime_ = now;

	if (keyPressed('r')) {
		// Instant restart, speedrunner style
		playerPos_ = EngineMath::Vector3(0.0f, 0.0f, 0.0f);
		velocity_ = EngineMath::Vector3(0.0f, 0.0f, 0.0f);
		score_ = 0;
		startRound();
	}

	updatePlayer(dt);
	updateCamera(dt);
	updatePickups();

	if (!victory_) {
		roundTime_ = now - roundStart_;
	}
	else if (now - victoryTime_ > victoryHold) {
		startRound();
	}

	// Popups live for 1.2s
	popups_.erase(std::remove_if(popups_.begin(), popups_.end(), [now](const Popup& p) { return now - p.born > 1.2f; }), popups_.end());
}

void ChuckWorldScene::updatePlayer(float dt) {
	InputManager& input = InputManager::getInstance();

	// ---------------------------------------------------------------- input -> wish direction
	// Movement is relative to the CAMERA, not the world: W always walks "into the screen"
	float forwardInput = 0.0f;
	float strafeInput = 0.0f;
	if (keyDown('w') || input.isSpecialKeyDown(GLUT_KEY_UP))    forwardInput += 1.0f;
	if (keyDown('s') || input.isSpecialKeyDown(GLUT_KEY_DOWN))  forwardInput -= 1.0f;
	if (keyDown('d') || input.isSpecialKeyDown(GLUT_KEY_RIGHT)) strafeInput += 1.0f;
	if (keyDown('a') || input.isSpecialKeyDown(GLUT_KEY_LEFT))  strafeInput -= 1.0f;

	const EngineMath::Vector3 camForward(-std::sin(cameraYaw_), 0.0f, -std::cos(cameraYaw_));
	const EngineMath::Vector3 camRight(std::cos(cameraYaw_), 0.0f, -std::sin(cameraYaw_));
	EngineMath::Vector3 wish = camForward * forwardInput + camRight * strafeInput;
	if (wish.lengthSquared() > 1.0f) {
		wish = wish.normalized(); // no diagonal speed boost (sorry, speedrunners)
	}

	// ---------------------------------------------------------------- horizontal motion
	const float blend = std::min(1.0f, dt * acceleration);
	velocity_.x_ += (wish.x_ * moveSpeed - velocity_.x_) * blend;
	velocity_.z_ += (wish.z_ * moveSpeed - velocity_.z_) * blend;

	playerPos_.x_ += velocity_.x_ * dt;
	playerPos_.z_ += velocity_.z_ * dt;

	// Face where we're going (smoothly, the short way around)
	if (wish.lengthSquared() > 0.01f) {
		const float target = std::atan2(-wish.x_, -wish.z_);
		facing_ += angleDelta(facing_, target) * std::min(1.0f, dt * 12.0f);
	}

	// ---------------------------------------------------------------- collisions
	// Crates: circle vs box. Find the closest point on the box footprint to the player;
	// if it's closer than the player's radius, shove the player out along that direction.
	// Skipped when the player's feet are above the crate (= standing on / jumping over it).
	for (const Crate& c : crates_) {
		if (playerPos_.y_ >= c.top - 0.05f) {
			continue;
		}
		const float closestX = clampf(playerPos_.x_, c.x - c.hx, c.x + c.hx);
		const float closestZ = clampf(playerPos_.z_, c.z - c.hz, c.z + c.hz);
		const float dx = playerPos_.x_ - closestX;
		const float dz = playerPos_.z_ - closestZ;
		const float distSq = dx * dx + dz * dz;

		if (distSq < playerRadius * playerRadius) {
			if (distSq > 1e-8f) {
				const float dist = std::sqrt(distSq);
				playerPos_.x_ += dx / dist * (playerRadius - dist);
				playerPos_.z_ += dz / dist * (playerRadius - dist);
			}
			else {
				// Center somehow ended up inside the box: pop out through the nearest side
				const float pushX = c.hx - std::fabs(playerPos_.x_ - c.x) + playerRadius;
				const float pushZ = c.hz - std::fabs(playerPos_.z_ - c.z) + playerRadius;
				if (pushX < pushZ) {
					playerPos_.x_ += playerPos_.x_ >= c.x ? pushX : -pushX;
				}
				else {
					playerPos_.z_ += playerPos_.z_ >= c.z ? pushZ : -pushZ;
				}
			}
		}
	}

	// Pillars: circle vs circle, the easiest collision there is
	for (const Pillar& p : pillars_) {
		const float dx = playerPos_.x_ - p.x;
		const float dz = playerPos_.z_ - p.z;
		const float minDist = playerRadius + p.radius;
		const float distSq = dx * dx + dz * dz;
		if (distSq < minDist * minDist && distSq > 1e-8f) {
			const float dist = std::sqrt(distSq);
			playerPos_.x_ = p.x + dx / dist * minDist;
			playerPos_.z_ = p.z + dz / dist * minDist;
		}
	}

	// The invisible arena wall
	const float fromCenter = std::sqrt(playerPos_.x_ * playerPos_.x_ + playerPos_.z_ * playerPos_.z_);
	if (fromCenter > arenaRadius) {
		playerPos_.x_ *= arenaRadius / fromCenter;
		playerPos_.z_ *= arenaRadius / fromCenter;
	}

	// ---------------------------------------------------------------- vertical motion
	if (grounded_ && (input.isPressed(' ') || keyPressed('j'))) {
		velocity_.y_ = jumpVelocity;
		grounded_ = false;
	}

	velocity_.y_ -= gravity * dt;
	playerPos_.y_ += velocity_.y_ * dt;

	const float ground = groundHeightAt(playerPos_.x_, playerPos_.z_, playerPos_.y_);
	shadowY_ = ground;
	if (playerPos_.y_ <= ground) {
		playerPos_.y_ = ground;
		velocity_.y_ = 0.0f;
		grounded_ = true;

		// Standing on the 3-unit boss tower = certified parkour legend
		if (ground >= 2.9f) {
			ChuckEdition::getInstance().unlock(Achievement::Parkour);
		}
	}
	else {
		grounded_ = false;
	}

	// ---------------------------------------------------------------- hand it to the bot
	if (player_) {
		player_->setPosition(playerPos_);
		player_->setFacing(facing_);
		const float horizontalSpeed = std::sqrt(velocity_.x_ * velocity_.x_ + velocity_.z_ * velocity_.z_);
		player_->setMotion(horizontalSpeed, grounded_);
	}
}

float ChuckWorldScene::groundHeightAt(float x, float z, float feetY) const {
	// The floor is 0. A crate counts as ground if we're over its top face AND its top isn't
	// way above our feet (that would be "standing inside the crate"). The stepUp tolerance
	// is what lets you land a jump that only *barely* cleared the edge.
	float ground = 0.0f;
	for (const Crate& c : crates_) {
		const bool over = std::fabs(x - c.x) <= c.hx + 0.12f && std::fabs(z - c.z) <= c.hz + 0.12f;
		if (over && c.top <= feetY + stepUp) {
			ground = std::max(ground, c.top);
		}
	}
	return ground;
}

void ChuckWorldScene::updateCamera(float dt) {
	InputManager& input = InputManager::getInstance();

	// Q/E orbit, right-drag orbit + tilt, scroll zoom
	if (keyDown('q')) cameraYaw_ += 2.2f * dt;
	if (keyDown('e')) cameraYaw_ -= 2.2f * dt;
	if (input.isMouseButtonDown(MOUSEBUTTON_RIGHT)) {
		cameraYaw_ -= input.mouseDeltaX() * 0.008f;
		cameraPitch_ = clampf(cameraPitch_ + input.mouseDeltaY() * 0.006f, 0.08f, 1.25f);
	}
	if (input.isMouseButtonPressed(MOUSEBUTTON_SCROLLUP)) {
		cameraDistance_ = std::max(3.0f, cameraDistance_ - 0.6f);
	}
	if (input.isMouseButtonPressed(MOUSEBUTTON_SCROLLDOWN)) {
		cameraDistance_ = std::min(14.0f, cameraDistance_ + 0.6f);
	}

	// The camera focus chases the player with a little lag = smooth, cinematic follow.
	// (dt == 0 on the very first frame snaps it into place instead.)
	const EngineMath::Vector3 goal = playerPos_ + EngineMath::Vector3(0.0f, 1.1f, 0.0f);
	const float follow = dt <= 0.0f ? 1.0f : std::min(1.0f, dt * 8.0f);
	cameraFocus_ += (goal - cameraFocus_) * follow;

	// ------------------------------------------------------------------------------
	//  CAMERA COLLISION (aka "why can I see inside the pillar")
	//  Walk along the boom from the player outward and stop at the first spot that's
	//  inside an obstacle. Pull IN instantly (never let the lens clip into a wall), but
	//  ease back OUT smoothly once the way is clear, so the camera doesn't jitter.
	// ------------------------------------------------------------------------------
	const EngineMath::Vector3 direction(
		std::sin(cameraYaw_) * std::cos(cameraPitch_),
		std::sin(cameraPitch_),
		std::cos(cameraYaw_) * std::cos(cameraPitch_)
	);

	float allowed = cameraDistance_;
	for (float d = 1.2f; d <= cameraDistance_; d += 0.2f) {
		if (cameraBlocked(cameraFocus_ + direction * d)) {
			allowed = std::max(1.2f, d - 0.3f);
			break;
		}
	}

	if (allowed < cameraBoom_ || dt <= 0.0f) {
		cameraBoom_ = allowed;
	}
	else {
		cameraBoom_ += (allowed - cameraBoom_) * std::min(1.0f, dt * 4.0f);
	}

	camera_.setTarget(cameraFocus_);
	camera_.setPosition(cameraFocus_ + direction * cameraBoom_);
}

bool ChuckWorldScene::cameraBlocked(const EngineMath::Vector3& p) const {
	const float lens = 0.3f; // keep the lens this far from any surface
	for (const Pillar& pillar : pillars_) {
		const float dx = p.x_ - pillar.x;
		const float dz = p.z_ - pillar.z;
		const float r = pillar.radius + lens;
		if (dx * dx + dz * dz < r * r && p.y_ < pillar.height + lens) {
			return true;
		}
	}
	for (const Crate& c : crates_) {
		if (std::fabs(p.x_ - c.x) < c.hx + lens && std::fabs(p.z_ - c.z) < c.hz + lens && p.y_ < c.top + lens) {
			return true;
		}
	}
	return false;
}

void ChuckWorldScene::updatePickups() {
	if (victory_) {
		return;
	}

	const float now = ChuckEdition::getInstance().time();
	const EngineMath::Vector3 chest = playerPos_ + EngineMath::Vector3(0.0f, 0.8f, 0.0f);

	int remaining = 0;
	for (PowerCore* core : cores_) {
		if (core->isCollected()) {
			continue;
		}

		if ((core->anchor() - chest).lengthSquared() < pickupRadius * pickupRadius) {
			core->collect();

			// Chain pickups inside the combo window for a growing multiplier
			combo_ = (now - lastPickupTime_ <= comboWindow) ? combo_ + 1 : 1;
			lastPickupTime_ = now;
			const int points = 100 * combo_;
			score_ += points;

			std::string text = "+" + std::to_string(points);
			if (combo_ > 1) {
				text += "  COMBO x" + std::to_string(combo_);
			}
			popups_.push_back(Popup{ core->anchor(), now, text });

			if (player_) {
				player_->celebrate();
			}

			ChuckEdition& chuck = ChuckEdition::getInstance();
			chuck.unlock(Achievement::PowerUp);
			chuck.awardXp(10 * combo_);
			if (combo_ >= 3) {
				chuck.killfeed("Chuck", "COMBO x" + std::to_string(combo_), "Power Core");
			}
			if (combo_ >= 5) {
				chuck.unlock(Achievement::ComboKing);
			}
			continue;
		}
		++remaining;
	}

	if (remaining == 0) {
		victory_ = true;
		victoryTime_ = now;
		roundTime_ = now - roundStart_;
		newBest_ = bestTime_ <= 0.0f || roundTime_ < bestTime_;
		if (newBest_) {
			bestTime_ = roundTime_;
		}
		// Speed bonus: finish under a minute and every second left over is worth 50
		score_ += std::max(0, static_cast<int>((60.0f - roundTime_) * 50.0f));

		ChuckEdition& chuck = ChuckEdition::getInstance();
		chuck.unlock(Achievement::VictoryRoyale);
		if (roundTime_ < 45.0f) {
			chuck.unlock(Achievement::Speedrunner);
		}
		chuck.killfeed("Chuck", "VICTORY", "Round " + std::to_string(round_));
		chuck.awardXp(200);
	}
}

void ChuckWorldScene::deInit() {
	player_ = nullptr;
	cores_.clear();
	crates_.clear();
	pillars_.clear();
	popups_.clear();
}

// ============================================================================ rendering extras

void ChuckWorldScene::drawWorldExtras() const {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Blob shadow: shrinks + fades as the player gets higher above the ground
	const float height = std::max(0.0f, playerPos_.y_ - shadowY_);
	const float radius = 0.45f / (1.0f + height * 0.6f);
	const float alpha = 0.6f / (1.0f + height * 0.8f);

	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.0f, 0.0f, 0.0f, alpha);
	glVertex3f(playerPos_.x_, shadowY_ + 0.01f, playerPos_.z_);
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i <= 24; ++i) {
		const float a = 2.0f * EngineMath::PI * i / 24.0f;
		glVertex3f(playerPos_.x_ + std::cos(a) * radius, shadowY_ + 0.01f, playerPos_.z_ + std::sin(a) * radius);
	}
	glEnd();

	// Each live core casts a pool of golden light on whatever it hovers over
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	for (const PowerCore* core : cores_) {
		if (core->isCollected()) {
			continue;
		}
		const EngineMath::Vector3& c = core->anchor();
		const float y = c.y_ - 0.74f;
		glBegin(GL_TRIANGLE_FAN);
		glColor4f(1.0f, 0.75f, 0.15f, 0.35f);
		glVertex3f(c.x_, y, c.z_);
		glColor4f(1.0f, 0.75f, 0.15f, 0.0f);
		for (int i = 0; i <= 20; ++i) {
			const float a = 2.0f * EngineMath::PI * i / 20.0f;
			glVertex3f(c.x_ + std::cos(a) * 0.7f, y, c.z_ + std::sin(a) * 0.7f);
		}
		glEnd();
	}

	glPopAttrib();
}

void ChuckWorldScene::drawHud(const GamerTheme::Theme& p, float time) const {
	using Render2D::Font;
	const GamerTheme::Rgb white = { 1.0f, 1.0f, 1.0f };
	const GamerTheme::Rgb black = { 0.0f, 0.0f, 0.0f };
	const GamerTheme::Rgb gold = { 1.0f, 0.8f, 0.15f };
	const float w = static_cast<float>(Render2D::width());
	const float h = static_cast<float>(Render2D::height());
	ChuckEdition& chuck = ChuckEdition::getInstance();

	// ---------------------------------------------------------------- floating world labels
	float sx = 0.0f;
	float sy = 0.0f;

	// Nametag over the player's head, like every online shooter ever
	if (chuck.worldToScreen(playerPos_ + EngineMath::Vector3(0.0f, 1.95f, 0.0f), sx, sy)) {
		const std::string tag = "[PRO] CHUCK";
		const float tw = static_cast<float>(Render2D::textWidth(tag, Font::Body));
		Render2D::chamferBox(sx - tw * 0.5f - 6.0f, sy - 13.0f, tw + 12.0f, 17.0f, 4.0f, black, 0.5f);
		Render2D::textGlow(sx - tw * 0.5f, sy, tag, Font::Body, white, p.accent, 1.0f);
		Render2D::progressBar(sx - 26.0f, sy + 7.0f, 52.0f, 3.0f, 1.0f, GamerTheme::Rgb{ 0.3f, 1.0f, 0.45f }, black, 0.9f);
	}

	// "+100 COMBO x3" popups float up and fade out from where the core was grabbed
	for (const Popup& popup : popups_) {
		const float age = time - popup.born;
		const float k = clampf(age / 1.2f, 0.0f, 1.0f);
		if (chuck.worldToScreen(popup.position + EngineMath::Vector3(0.0f, 0.4f + k * 1.2f, 0.0f), sx, sy)) {
			const float tw = Render2D::strokeTextWidth(popup.text, 16.0f, 1.0f);
			Render2D::strokeTextNeon(sx - tw * 0.5f, sy, popup.text, 16.0f, gold, 1.0f - k * k, 1.0f);
		}
	}

	// Waypoint marker on the nearest remaining core
	const PowerCore* nearest = nullptr;
	float nearestDist = 1e9f;
	for (const PowerCore* core : cores_) {
		if (core->isCollected()) {
			continue;
		}
		const float d = (core->anchor() - playerPos_).length();
		if (d < nearestDist) {
			nearestDist = d;
			nearest = core;
		}
	}
	if (nearest && !victory_ && chuck.worldToScreen(nearest->anchor() + EngineMath::Vector3(0.0f, 0.7f, 0.0f), sx, sy)) {
		const float bounce = std::sin(time * 5.0f) * 3.0f;
		Render2D::line(sx - 6.0f, sy - 12.0f + bounce, sx, sy - 4.0f + bounce, gold, 0.95f, 2.0f);
		Render2D::line(sx + 6.0f, sy - 12.0f + bounce, sx, sy - 4.0f + bounce, gold, 0.95f, 2.0f);
		const std::string meters = std::to_string(static_cast<int>(nearestDist)) + "m";
		Render2D::textShadow(sx - Render2D::textWidth(meters, Font::Tiny) * 0.5f, sy - 16.0f + bounce, meters, Font::Tiny, gold, 1.0f);
	}

	// ---------------------------------------------------------------- objective panel (top center)
	int collected = 0;
	for (const PowerCore* core : cores_) {
		collected += core->isCollected() ? 1 : 0;
	}

	const float pw = 230.0f;
	const float px = (w - pw) * 0.5f;
	const float py = 22.0f;
	Render2D::chamferBox(px, py, pw, 58.0f, 10.0f, GamerTheme::scale(p.skyTop, 0.6f), 0.65f);
	Render2D::chamferOutline(px, py, pw, 58.0f, 10.0f, gold, 0.7f);
	const std::string title = "CHUCK WORLD // ROUND " + std::to_string(round_);
	const float titleW = static_cast<float>(Render2D::textWidth(title, Font::Tiny)) + 16.0f;
	Render2D::chamferBox(px, py - 12.0f, titleW, 13.0f, 5.0f, gold, 0.9f);
	Render2D::text(px + 8.0f, py - 2.0f, title, Font::Tiny, black, 1.0f);

	// One little diamond per core: lit when collected
	for (int i = 0; i < coresPerRound; ++i) {
		const float cx = px + 18.0f + i * 21.0f;
		const float cy = py + 15.0f;
		const bool lit = i < collected;
		glColor4f(gold.r, gold.g, gold.b, lit ? 1.0f : 0.18f);
		glBegin(GL_QUADS);
		glVertex2f(cx, cy - 6.0f);
		glVertex2f(cx + 6.0f, cy);
		glVertex2f(cx, cy + 6.0f);
		glVertex2f(cx - 6.0f, cy);
		glEnd();
	}

	Render2D::text(px + 12.0f, py + 38.0f, "TIME " + formatTime(roundTime_), Font::MonoSmall, white, 0.95f);
	Render2D::text(px + 12.0f, py + 52.0f, "BEST " + (bestTime_ > 0.0f ? formatTime(bestTime_) : std::string("--:--.-")), Font::MonoSmall, white, 0.55f);
	const std::string scoreText = std::to_string(score_);
	const float scoreW = Render2D::strokeTextWidth(scoreText, 16.0f, 1.0f);
	Render2D::strokeTextNeon(px + pw - 14.0f - scoreW, py + 50.0f, scoreText, 16.0f, gold, 1.0f, 1.0f);
	Render2D::text(px + pw - 14.0f - scoreW - 40.0f, py + 50.0f, "SCORE", Font::Tiny, gold, 0.7f);

	// Combo meter: drains over the combo window
	const float comboLeft = 1.0f - (time - lastPickupTime_) / comboWindow;
	if (combo_ > 1 && comboLeft > 0.0f && !victory_) {
		const std::string combo = "COMBO x" + std::to_string(combo_);
		const float cw = Render2D::strokeTextWidth(combo, 14.0f, 1.0f);
		Render2D::strokeTextNeon((w - cw) * 0.5f, py + 82.0f, combo, 14.0f, p.accent2, 1.0f, 1.0f);
		Render2D::progressBar((w - 120.0f) * 0.5f, py + 88.0f, 120.0f, 3.0f, comboLeft, p.accent2, black, 0.9f);
	}

	// ---------------------------------------------------------------- controls hint (bottom center)
	const std::string hint = "WASD MOVE  |  SPACE JUMP  |  Q/E or R-DRAG CAMERA  |  SCROLL ZOOM  |  R RESTART";
	const float hw = static_cast<float>(Render2D::textWidth(hint, Font::Tiny));
	Render2D::textShadow((w - hw) * 0.5f, h - 62.0f, hint, Font::Tiny, white, 0.6f);

	// ---------------------------------------------------------------- VICTORY ROYALE
	if (victory_) {
		const float age = time - victoryTime_;
		const float fadeIn = clampf(age / 0.35f, 0.0f, 1.0f);
		const float fadeOut = clampf((victoryHold - age) / 0.6f, 0.0f, 1.0f);
		const float a = fadeIn * fadeOut;

		Render2D::rectGradientV(0.0f, h * 0.30f, w, h * 0.12f, black, 0.0f, black, 0.55f * a);
		Render2D::rectGradientV(0.0f, h * 0.42f, w, h * 0.12f, black, 0.55f * a, black, 0.0f);

		// Punch-in: the title starts big and slams down to size
		const float cap = 38.0f + (1.0f - fadeIn) * 30.0f;
		const std::string banner = "VICTORY ROYALE";
		const float bw = Render2D::strokeTextWidth(banner, cap, 3.0f);
		Render2D::strokeTextNeon((w - bw) * 0.5f, h * 0.44f, banner, cap, gold, a, 3.0f);

		const std::string sub = "ALL CORES SECURED IN " + formatTime(roundTime_);
		const float sw = static_cast<float>(Render2D::textWidth(sub, Font::Large));
		Render2D::textGlow((w - sw) * 0.5f, h * 0.44f + 30.0f, sub, Font::Large, white, p.accent, a);

		if (newBest_ && std::fmod(age, 0.5f) < 0.33f) {
			const std::string best = "NEW PERSONAL BEST";
			const float bw2 = static_cast<float>(Render2D::textWidth(best, Font::Body));
			Render2D::text((w - bw2) * 0.5f, h * 0.44f + 50.0f, best, Font::Body, p.accent2, a);
		}
	}
}
