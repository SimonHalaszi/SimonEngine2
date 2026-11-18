#ifndef ELLIPSOID_HPP
#define ELLIPSOID_HPP

#include "Primitive.hpp"

class Ellipsoid : public Primitive {
	public:
		Ellipsoid() : Ellipsoid(EngineMath::Transform{}) {}
		Ellipsoid(const EngineMath::Transform& localTransform, const EngineRender::Material& material = {}, int slices = 3, int stacks = 2);

	private:
		int slices_;
		int stacks_;
		virtual const void resolveMeshes() override;
};

#endif 