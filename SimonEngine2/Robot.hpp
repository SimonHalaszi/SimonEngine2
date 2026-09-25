#ifndef ROBOT_HPP
#define ROBOT_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Robot : public Object {
	public:
		Robot() = default;
		Robot(const EngineMath::Transform& localTransform);

		void onStart() override;
		void update() override;

	private:
		EngineMath::Vector3 leftArmPivot_;
		EngineMath::Vector3 rightArmPivot_;
};

#endif 
