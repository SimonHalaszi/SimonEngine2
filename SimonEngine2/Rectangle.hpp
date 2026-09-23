#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Rectangle : public Object {
	public:
		Rectangle() = default;
		Rectangle(const EngineMath::Transform& transform, const EngineUtil::ColorRGB& color);

		void onStart() override;
		void draw() override;
		void update() override;
		void onDestruction() override {}

	private:
		bool drawAsSolid_ = false;

		void drawSolid() const;
		void drawEdges() const;

		EngineUtil::ColorRGB color_;

		float rotationRadians_ = 0.0f;
		float rotationSpeed_ = 0.02f;
};

#endif 
