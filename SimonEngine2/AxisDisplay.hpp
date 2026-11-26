#ifndef AXIS_DISPLAY_HPP
#define AXIS_DISPLAY_HPP

#include "Object.hpp"

class AxisDisplay : public Object {
	public:
		AxisDisplay();

	protected:
		void onStart() override final;
		void update() override final;
};

#endif 
