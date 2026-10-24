#ifndef RECTANGULAR_PRISM_HPP
#define RECTANGULAR_PRISM_HPP

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class RectangularPrism : public Primitive {
	public:
		RectangularPrism() = default;
		RectangularPrism(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color);

	private:
		const EngineUtil::Mesh* mesh_ = nullptr;
		void drawSolid() const override final;
		void drawEdges() const override final;
};

#endif 