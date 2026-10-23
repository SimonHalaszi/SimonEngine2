#include "PowerCore.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

#include "EngineUtil.hpp"
#include "RectangularPrism.hpp"
#include "Sphere.hpp"
#include "Cylinder.hpp"

namespace {
	constexpr float collectDuration = 0.25f;

	float nowSeconds() {
		using clock = std::chrono::steady_clock;
		static const clock::time_point start = clock::now();
		return std::chrono::duration<float>(clock::now() - start).count();
	}
}

PowerCore::PowerCore(const EngineMath::Vector3& position) : anchor_(position) {
	localTransform_.position_ = position;
	name_ = "PowerCore";
	tag_ = "Collectible";
	// Each core bobs on its own rhythm, seeded from where it spawned
	seed_ = position.x_ * 1.7f + position.z_ * 2.3f;
}

void PowerCore::onStart() {
	// The crystal: a gold cube tipped onto its corner. A 45 degree roll then a ~35.26 degree
	// pitch (atan(1/sqrt(2))) stands it perfectly on its vertex = diamond silhouette.
	std::unique_ptr<Object> crystal = std::make_unique<Object>();
	crystal_ = crystal.get();

	crystal->addChild(std::make_unique<RectangularPrism>(
		EngineMath::Transform(
			EngineMath::Vector3(0.0f, 0.0f, 0.0f),
			EngineMath::Quaternion::fromXYZ(EngineMath::degreesToRadians(35.26f), 0.0f, EngineMath::degreesToRadians(45.0f)),
			EngineMath::Vector3(0.32f, 0.32f, 0.32f)
		),
		EngineUtil::ColorRGB(255, 205, 40)
	));

	// The white-hot core peeking through the corners
	crystal->addChild(std::make_unique<Sphere>(
		EngineMath::Transform(
			EngineMath::Vector3(0.0f, 0.0f, 0.0f),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(0.36f, 0.36f, 0.36f)
		),
		EngineUtil::ColorRGB(255, 250, 220),
		10,
		8
	));
	addChild(std::move(crystal));

	// A flat, wide, paper-thin cylinder = an orbit ring
	std::unique_ptr<Object> ring = std::make_unique<Object>();
	ring_ = ring.get();
	ring->addChild(std::make_unique<Cylinder>(
		EngineMath::Transform(
			EngineMath::Vector3(0.0f, 0.0f, 0.0f),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(0.75f, 0.015f, 0.75f)
		),
		EngineUtil::ColorRGB(255, 120, 40),
		24
	));
	addChild(std::move(ring));
}

void PowerCore::collect() {
	if (collected_) {
		return;
	}
	collected_ = true;
	collectTimer_ = collectDuration;
}

void PowerCore::respawn(const EngineMath::Vector3& position) {
	anchor_ = position;
	collected_ = false;
	collectTimer_ = 0.0f;
	setScale(EngineMath::Vector3(1.0f, 1.0f, 1.0f));
	if (!isDrawing()) {
		toggleDrawing();
	}
}

void PowerCore::update() {
	const float now = nowSeconds();
	const float dt = lastTime_ < 0.0f ? 0.0f : std::min(now - lastTime_, 0.05f);
	lastTime_ = now;

	spin_ += dt * 2.2f;

	if (collected_) {
		if (collectTimer_ > 0.0f) {
			// Pop: overshoot bigger for a frame, then shrink to nothing
			collectTimer_ = std::max(0.0f, collectTimer_ - dt);
			const float k = collectTimer_ / collectDuration;     // 1 -> 0
			const float s = std::sin(k * EngineMath::PI) * 0.6f + k;
			setScale(EngineMath::Vector3(s, s, s));
			setPosition(anchor_ + EngineMath::Vector3(0.0f, (1.0f - k) * 0.8f, 0.0f));
		}
		else if (isDrawing()) {
			toggleDrawing();
		}
		return;
	}

	// Idle: bob up and down, crystal spins one way, ring tilts and spins the other
	const float bob = std::sin(now * 2.0f + seed_) * 0.12f;
	setPosition(anchor_ + EngineMath::Vector3(0.0f, bob, 0.0f));

	if (crystal_) {
		crystal_->setRotation(EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(0.0f, 1.0f, 0.0f), spin_));
	}
	if (ring_) {
		ring_->setRotation(
			EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(0.0f, 1.0f, 0.0f), -spin_ * 0.7f) *
			EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(1.0f, 0.0f, 0.0f), 0.35f + 0.15f * std::sin(now + seed_))
		);
	}
}
