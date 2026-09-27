#ifndef LINE_HPP
#define LINE_HPP

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Line : public Primitive {
	public:
		Line() = default;
		Line(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) : Primitive(localTransform, color) {}

	private:
		void drawSolid() const override;
		void drawEdges() const override;
};

#endif 
