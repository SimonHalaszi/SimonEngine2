#ifndef PLANE_HPP
#define PLANE_HPP

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Plane : public Primitive {
	public:
		Plane() = default;
		Plane(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) : Primitive(localTransform, color) {}

	private:
		void drawSolid() const override final;
		void drawEdges() const override final;
};

#endif 