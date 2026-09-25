#ifndef RECTANGLE_OBJECT_HPP
#define RECTANGULAR_PRISM_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class RectangularPrism : public Object {
	public:
		RectangularPrism() = default;
		RectangularPrism(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color);

		void onStart() override {}
		void draw() override;
		void update() override;
		void onDestruction() override {}

	private:
		bool drawAsSolid_ = false;

		void drawSolid() const;
		void drawEdges() const;

		EngineUtil::ColorRGB color_;
};

#endif 
