#ifndef AXIS_DISPLAY_HPP
#define AXIS_DISPLAY_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class AxisDisplay : public Object {
	public:
		AxisDisplay() = default;
		AxisDisplay(const EngineMath::Transform& localTransform);

		void onStart() override;
		void update() override;
};

#endif 
