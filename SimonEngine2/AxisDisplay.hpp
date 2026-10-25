#ifndef AXIS_DISPLAY_HPP
#define AXIS_DISPLAY_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class AxisDisplay : public Object {
	public:
		AxisDisplay();

		void onStart() override final;
		void update() override final;
};

#endif 
