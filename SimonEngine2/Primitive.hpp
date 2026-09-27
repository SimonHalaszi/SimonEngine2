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

		void setColor(EngineUtil::ColorRGB);

	protected:
		bool drawAsSolid_ = true;

		virtual void drawSolid() const = 0;
		virtual void drawEdges() const = 0;

		bool drawColor_ = true;
		EngineUtil::ColorRGB activeColor_;
		EngineUtil::ColorRGB color_;
		EngineUtil::ColorRGB grayColor_;
};

#endif 
