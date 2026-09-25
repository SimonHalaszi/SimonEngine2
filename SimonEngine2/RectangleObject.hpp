#ifndef RECTANGLE_OBJECT_HPP
#define RECTANGLE_OBJECT_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class RectangleObject : public Object {
	public:
		RectangleObject() = default;
		RectangleObject(const EngineMath::Transform& transform, const EngineUtil::ColorRGB& color);

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
