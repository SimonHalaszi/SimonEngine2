#ifndef RECTANGLE_OBJECT_HPP
#define RECTANGULAR_PRISM_HPP

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class RectangularPrism : public Primitive {
	public:
		RectangularPrism() = default;
		RectangularPrism(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) : Primitive(localTransform, color) {}

	private:
		void drawSolid() const override;
		void drawEdges() const override;
};

#endif 
