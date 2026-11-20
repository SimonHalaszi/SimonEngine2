#ifndef ROBOT_HPP
#define ROBOT_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

#include "Limb.hpp"

class Robot : public Object {
	public:
		Robot() = default;
		Robot(const EngineMath::Transform& localTransform, const EngineRender::Material* material);
		void toggleAnimation() { isAnimating_ = !isAnimating_; }

	protected:
		void onStart() override final;
		void update() override final;

	private:
		Limb* leftArm_ = nullptr;
		Limb* rightArm_ = nullptr;
		Limb* leftLeg_ = nullptr;
		Limb* rightLeg_ = nullptr;
		const EngineRender::Material* material_ = nullptr;

		bool isAnimating_;

		float rotationStep_;
};

#endif 
