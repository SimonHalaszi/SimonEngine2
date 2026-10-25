#ifndef ELLIPSOID_HPP
#define ELLIPSOID_HPP

#include "Primitive.hpp"

class Ellipsoid : public Primitive {
	public:
		Ellipsoid() : Ellipsoid(EngineMath::Transform{}, EngineUtil::ColorRGB{}) {}
		Ellipsoid(
			const EngineMath::Transform& localTransform,
			const EngineUtil::ColorRGB& color,
			int slices = 3,
			int stacks = 2,
			const std::string& vertPath = "../Shaders/Basic.vert",
			const std::string& fragPath = "../Shaders/Color.frag"
		);

	private:
		int slices_;
		int stacks_;
		virtual const void resolveMeshes() override;
};

#endif 