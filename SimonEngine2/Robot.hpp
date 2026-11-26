#ifndef ROBOT_HPP
#define ROBOT_HPP

#include "EngineMath.hpp"
#include "EngineRender.hpp"

#include "Object.hpp"

class Limb;

class Robot : public Object {
	public:
		Robot(const EngineMath::Transform& localTransform,
			const EngineRender::Material* skin,
			const EngineRender::Material* shirt,
			const EngineRender::Material* pants
		);

	protected:
		void onStart() override final;
		void update() override final;

	private:
		Limb* leftArm_ = nullptr;
		Limb* rightArm_ = nullptr;
		Limb* leftLeg_ = nullptr;
		Limb* rightLeg_ = nullptr;
		const EngineRender::Material* skin_ = nullptr;
		const EngineRender::Material* shirt_ = nullptr;
		const EngineRender::Material* pants_ = nullptr;

		void toggleAnimation() { isAnimating_ = !isAnimating_; }
		bool isAnimating_;

		float rotationStep_;
};

#endif 
