#ifndef ROBOT_HPP
#define ROBOT_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"
#include "Limb.hpp"
#include "RectangularPrism.hpp"
#include "Sphere.hpp"

class Robot : public Object {
	public:
		Robot() = default;
		Robot(const EngineMath::Transform& localTransform, EngineUtil::ColorRGB color);

		void onStart() override final;
		void update() override final;

		void setColor(EngineUtil::ColorRGB);

	private:
		Limb* leftArm_ = nullptr;
		Limb* rightArm_ = nullptr;
		Limb* leftLeg_ = nullptr;
		Limb* rightLeg_ = nullptr;
		EngineUtil::ColorRGB color_;

		float rotationStep;
};

#endif 
