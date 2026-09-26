#ifndef LINE_HPP
#define LINE_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Line : public Object {
	public:
		Line() = default;
		Line(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color);

		void draw() override;
		void update() override;

	private:
		bool drawAsSolid_ = true;

		void drawSolid() const;
		void drawEdges() const;

		bool drawColor_ = true;
		EngineUtil::ColorRGB activeColor_;
		EngineUtil::ColorRGB color_;
		EngineUtil::ColorRGB grayColor_;
};

#endif 
