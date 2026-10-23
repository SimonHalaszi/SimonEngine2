#ifndef CHUCK_BOT_HPP
#define CHUCK_BOT_HPP

/*
 *    ___ _  _ _   _  ___ _  _____  ___ _____
 *   / __| || | | | |/ __| |/ / _ )/ _ \_   _|    SimonEngine2 :: CHUCK SPECIAL EDITION
 *  | (__| __ | |_| | (__| ' <| _ \ (_) || |      The playable hero of Chuck World.
 *   \___|_||_|\___/ \___|_|\_\___/\___/ |_|      Gold armor. Purple limbs. Zero fear.
 *
 *  Built entirely out of SimonEngine2's own primitives and Object hierarchy, which makes
 *  him a pretty good demo of the engine itself:
 *
 *      ChuckBot (root: position + facing)
 *       |- torso, belt, chest light              RectangularPrism
 *       |- head, antenna tip                     Sphere
 *       |- visor                                 RectangularPrism
 *       |- antenna                               Cylinder
 *       |- hip pivot L/R    -> leg prism         (empty Object used as a joint)
 *       '- shoulder pivot L/R -> arm prism       (empty Object used as a joint)
 *
 *  The "pivot" trick: a limb prism is centered on its own origin, so rotating it directly
 *  would spin it around its middle. Instead each limb hangs off an invisible parent Object
 *  placed exactly at the joint, offset downward by half its length. Rotate the parent and
 *  the limb swings from the hip/shoulder like a real leg. Hierarchical transforms FTW.
 *
 *  The bot doesn't read input or do physics. ChuckWorldScene moves him around and tells
 *  him how fast he's going; he just animates accordingly (walk cycle, idle breathing,
 *  airborne tuck).
 */

#include "Object.hpp"
#include "EngineMath.hpp"

class ChuckBot : public Object {
	public:
		ChuckBot() = default;
		explicit ChuckBot(const EngineMath::Vector3& position);

		// Set by the scene every tick, consumed by the animation
		void setMotion(float horizontalSpeed, bool grounded) { speed_ = horizontalSpeed; grounded_ = grounded; }

		// Facing angle around Y, in radians (0 = looking down -Z)
		void setFacing(float yawRadians);
		float getFacing() const { return facing_; }

		// A little happy hop of the arms when a core gets collected
		void celebrate();

	protected:
		void onStart() override final;
		void update() override final;

	private:
		Object* hipLeft_ = nullptr;
		Object* hipRight_ = nullptr;
		Object* shoulderLeft_ = nullptr;
		Object* shoulderRight_ = nullptr;
		Object* torsoRoot_ = nullptr; // everything above the hips hangs off this so it can bob

		float speed_ = 0.0f;
		bool grounded_ = true;
		float facing_ = 0.0f;

		float walkPhase_ = 0.0f;
		float walkBlend_ = 0.0f;    // 0 = standing, 1 = full stride (smoothed)
		float idleClock_ = 0.0f;
		float celebrateTimer_ = 0.0f;
		float lastTime_ = -1.0f;
};

#endif
