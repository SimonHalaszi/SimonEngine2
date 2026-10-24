#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include "Object.hpp"
#include "InputManager.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Primitive : public Object {
	public:
		Primitive() = default;
		Primitive(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color);

		void draw() override;
		void update() override;

		void setColor(const EngineUtil::ColorRGB& color) { color_ = color; }

	protected:
		bool drawAsSolid_ = true;
		EngineUtil::ColorRGB color_;

		virtual void drawSolid() const = 0;
		virtual void drawEdges() const = 0;
};

#endif 
