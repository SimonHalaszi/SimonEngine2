#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <algorithm>

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Sphere : public Primitive {
	public:
		Sphere() = default;
		// The diameter is localTransform.scale_.x_; Radius is equal to localTransform.scale_.x_ / 2.0f; y_ and z_ values are not taken into consideration
		Sphere(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices, int stacks);

	private:
		float radius_ = 0.5f;
		int slices_ = 16;
		int stacks_ = 16;

		void buildMesh();
		const EngineUtil::Vertex& vertexAt(int stack, int slice) const;
		std::vector<EngineUtil::Vertex> vertices_;

		void drawSolid() const override;
		void drawEdges() const override;
};

#endif 
