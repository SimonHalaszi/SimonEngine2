#ifndef POWER_CORE_HPP
#define POWER_CORE_HPP

/*
 *   ___  _____      _____ ___    ___ ___  ___ ___
 *  | _ \/ _ \ \    / / __| _ \  / __/ _ \| _ \ __|    SimonEngine2 :: CHUCK SPECIAL EDITION
 *  |  _/ (_) \ \/\/ /| _||   / | (_| (_) |   / _|     The collectible. Spins. Bobs. Glows.
 *  |_|  \___/ \_/\_/ |___|_|_\  \___\___/|_|_\___|    Wants to be picked up very badly.
 *
 *  A diamond-shaped crystal (a cube balanced on its corner) with a hot core inside and a
 *  thin orbit ring around it. Touch it and it gets "collected": it shrinks to nothing,
 *  hides, and waits for the next round to respawn somewhere else.
 */

#include "Object.hpp"
#include "EngineMath.hpp"

class PowerCore : public Object {
	public:
		PowerCore() = default;
		explicit PowerCore(const EngineMath::Vector3& position);

		bool isCollected() const { return collected_; }

		// Plays the pickup animation and hides the core
		void collect();

		// Brings it back at a new spot for the next round
		void respawn(const EngineMath::Vector3& position);

		// Where it floats around (the bob animation moves the drawn model, not this)
		const EngineMath::Vector3& anchor() const { return anchor_; }

	protected:
		void onStart() override final;
		void update() override final;

	private:
		EngineMath::Vector3 anchor_;
		Object* crystal_ = nullptr;
		Object* ring_ = nullptr;

		bool collected_ = false;
		float collectTimer_ = 0.0f; // counts down the shrink animation
		float spin_ = 0.0f;
		float seed_ = 0.0f;         // desyncs the bobbing between cores
		float lastTime_ = -1.0f;
};

#endif
