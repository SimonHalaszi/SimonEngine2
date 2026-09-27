#ifndef ROBOT_HPP
#define ROBOT_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Robot : public Object {
	public:
		Robot() = default;
		Robot(const EngineMath::Transform& localTransform, EngineUtil::ColorRGB color);

		void onStart() override;
		void update() override;

		void changeColor(EngineUtil::ColorRGB);

	private:
		EngineMath::Vector3 leftArmPivot_;
		EngineMath::Vector3 rightArmPivot_;
		EngineUtil::ColorRGB color_;
};

#endif 
