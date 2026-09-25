#ifndef LINE_HPP
#define LINE_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Line : public Object {
	public:
		Line() = default;
		Line(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color);

		void draw() override;

	private:
		EngineUtil::ColorRGB color_;
};

#endif 
