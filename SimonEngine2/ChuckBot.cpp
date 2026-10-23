#include "ChuckBot.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "EngineUtil.hpp"
#include "RectangularPrism.hpp"
#include "Sphere.hpp"
#include "Cylinder.hpp"

namespace {
	// ---- proportions (world units). He's ~1.6 units tall, antenna included.
	constexpr float hipHeight = 0.50f;
	constexpr float legLength = 0.48f;
	constexpr float torsoHeight = 0.55f;
	constexpr float shoulderHeight = hipHeight + torsoHeight - 0.06f;
	constexpr float armLength = 0.44f;

	// ---- the drip
	const EngineUtil::ColorRGB armorGold(255, 196, 40);
	const EngineUtil::ColorRGB limbPurple(140, 60, 255);
	const EngineUtil::ColorRGB headWhite(235, 235, 245);
	const EngineUtil::ColorRGB visorCyan(30, 235, 255);
	const EngineUtil::ColorRGB darkTrim(40, 30, 70);
	const EngineUtil::ColorRGB antennaRed(255, 60, 110);

	EngineMath::Transform box(float x, float y, float z, float sx, float sy, float sz) {
		return EngineMath::Transform(
			EngineMath::Vector3(x, y, z),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(sx, sy, sz)
		);
	}

	// Real-time seconds (animation should not care how fast the update timer ticks)
	float nowSeconds() {
		using clock = std::chrono::steady_clock;
		static const clock::time_point start = clock::now();
		return std::chrono::duration<float>(clock::now() - start).count();
	}

	// Makes an invisible joint at `position` with a limb prism hanging below it
	std::unique_ptr<Object> makeJoint(const EngineMath::Vector3& position, float length, float thickness, const EngineUtil::ColorRGB& color, Object*& outJoint) {
		std::unique_ptr<Object> joint = std::make_unique<Object>();
		joint->setPosition(position);
		joint->addChild(std::make_unique<RectangularPrism>(
			box(0.0f, -length * 0.5f, 0.0f, thickness, length, thickness * 1.1f), color
		));
		outJoint = joint.get();
		return joint;
	}
}

ChuckBot::ChuckBot(const EngineMath::Vector3& position) {
	localTransform_.position_ = position;
	name_ = "ChuckBot";
	tag_ = "Player";
}

void ChuckBot::setFacing(float yawRadians) {
	facing_ = yawRadians;
	setRotation(EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(0.0f, 1.0f, 0.0f), facing_));
}

void ChuckBot::celebrate() {
	celebrateTimer_ = 0.45f;
}

void ChuckBot::onStart() {
	// ---------------------------------------------------------------- legs (from the hips)
	addChild(makeJoint(EngineMath::Vector3(-0.13f, hipHeight, 0.0f), legLength, 0.17f, limbPurple, hipLeft_));
	addChild(makeJoint(EngineMath::Vector3(0.13f, hipHeight, 0.0f), legLength, 0.17f, limbPurple, hipRight_));

	// ---------------------------------------------------------------- upper body
	// Everything from the waist up hangs off one root so the walk cycle can bob it
	std::unique_ptr<Object> upper = std::make_unique<Object>();
	upper->setPosition(EngineMath::Vector3(0.0f, hipHeight, 0.0f));
	torsoRoot_ = upper.get();

	// torso, belt, glowing chest light (front of the bot is -Z)
	upper->addChild(std::make_unique<RectangularPrism>(box(0.0f, torsoHeight * 0.5f, 0.0f, 0.52f, torsoHeight, 0.32f), armorGold));
	upper->addChild(std::make_unique<RectangularPrism>(box(0.0f, 0.04f, 0.0f, 0.56f, 0.09f, 0.36f), darkTrim));
	upper->addChild(std::make_unique<RectangularPrism>(box(0.0f, torsoHeight * 0.62f, -0.165f, 0.16f, 0.12f, 0.03f), visorCyan));

	// head + visor
	const float headY = torsoHeight + 0.24f;
	upper->addChild(std::make_unique<Sphere>(box(0.0f, headY, 0.0f, 0.42f, 0.42f, 0.42f), headWhite, 16, 12));
	upper->addChild(std::make_unique<RectangularPrism>(box(0.0f, headY + 0.02f, -0.17f, 0.30f, 0.10f, 0.10f), visorCyan));

	// antenna with a blinky tip, because every robot needs one (Simon's Cylinder finally gets used!)
	upper->addChild(std::make_unique<Cylinder>(box(0.0f, headY + 0.32f, 0.0f, 0.03f, 0.22f, 0.03f), darkTrim, 8));
	upper->addChild(std::make_unique<Sphere>(box(0.0f, headY + 0.45f, 0.0f, 0.08f, 0.08f, 0.08f), antennaRed, 8, 6));

	// arms (from the shoulders, relative to the waist)
	upper->addChild(makeJoint(EngineMath::Vector3(-0.34f, shoulderHeight - hipHeight, 0.0f), armLength, 0.13f, limbPurple, shoulderLeft_));
	upper->addChild(makeJoint(EngineMath::Vector3(0.34f, shoulderHeight - hipHeight, 0.0f), armLength, 0.13f, limbPurple, shoulderRight_));

	addChild(std::move(upper));
}

void ChuckBot::update() {
	const float now = nowSeconds();
	const float dt = lastTime_ < 0.0f ? 0.0f : std::min(now - lastTime_, 0.05f);
	lastTime_ = now;

	// Blend toward "walking" when moving, toward "standing" when not. The blend keeps the
	// legs from snapping straight the instant you let go of W.
	const float targetBlend = grounded_ ? std::min(speed_ / 4.0f, 1.0f) : 0.0f;
	walkBlend_ += (targetBlend - walkBlend_) * std::min(1.0f, dt * 10.0f);

	// Stride frequency scales with speed so the feet don't moonwalk
	walkPhase_ += dt * (4.0f + speed_ * 2.2f);
	idleClock_ += dt;
	celebrateTimer_ = std::max(0.0f, celebrateTimer_ - dt);

	const EngineMath::Vector3 xAxis(1.0f, 0.0f, 0.0f);
	const EngineMath::Vector3 zAxis(0.0f, 0.0f, 1.0f);

	const float swing = std::sin(walkPhase_) * 0.75f * walkBlend_;

	float legL = swing;
	float legR = -swing;
	float armL = -swing * 0.9f;
	float armR = swing * 0.9f;
	float armSpread = 0.08f;

	if (!grounded_) {
		// Airborne: knees up, arms out. Peak gamer jump form.
		legL = 0.55f;
		legR = -0.25f;
		armL = -0.4f;
		armR = -0.4f;
		armSpread = 0.6f;
	}

	if (celebrateTimer_ > 0.0f) {
		// Arms straight up for a split second. +100 HYPE
		const float k = std::sin((celebrateTimer_ / 0.45f) * EngineMath::PI);
		armL = -2.6f * k;
		armR = -2.6f * k;
	}

	// Limbs hang along -Y, so a positive rotation around X swings the foot FORWARD (-Z),
	// which is the way the bot faces. (Rx: y' = y cos - z sin, z' = y sin + z cos)
	if (hipLeft_) hipLeft_->setRotation(EngineMath::Quaternion::fromAxisAngle(xAxis, legL));
	if (hipRight_) hipRight_->setRotation(EngineMath::Quaternion::fromAxisAngle(xAxis, legR));
	if (shoulderLeft_) shoulderLeft_->setRotation(EngineMath::Quaternion::fromAxisAngle(xAxis, armL) * EngineMath::Quaternion::fromAxisAngle(zAxis, -armSpread));
	if (shoulderRight_) shoulderRight_->setRotation(EngineMath::Quaternion::fromAxisAngle(xAxis, armR) * EngineMath::Quaternion::fromAxisAngle(zAxis, armSpread));

	// Upper body: bob twice per stride while walking, slow breathing while idle
	if (torsoRoot_) {
		const float bob = std::fabs(std::sin(walkPhase_)) * 0.05f * walkBlend_;
		const float breathe = std::sin(idleClock_ * 2.0f) * 0.012f * (1.0f - walkBlend_);
		torsoRoot_->setPosition(EngineMath::Vector3(0.0f, hipHeight + bob + breathe, 0.0f));
		// lean into the run a little
		torsoRoot_->setRotation(EngineMath::Quaternion::fromAxisAngle(xAxis, -0.12f * walkBlend_));
	}
}
