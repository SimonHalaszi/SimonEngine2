#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <algorithm>

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Sphere : public Primitive {
	public:
		Sphere() = default;
		// X, Y, Z should be equal in scale
		Sphere(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices, int stacks);

	private:
		int slices_ = 16;
		int stacks_ = 16;

		const EngineUtil::Vertex& vertexAt(int stack, int slice) const;
		const EngineUtil::Mesh* mesh_ = nullptr;

		void drawSolid() const override;
		void drawEdges() const override;
};

#endif 