#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "Object.hpp"
#include "Renderer.hpp"

#include "EngineMath.hpp"
#include "EngineRender.hpp"
#include "EngineUtil.hpp"


class Light : public Object {
	public:
		Light() = default;
		Light(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color = {}, float intensity = 1.0f)
			: color_(color), intensity_(intensity) {
			localTransform_ = localTransform;
		}

		void setColor(const EngineUtil::ColorRGB& color) { color_ = color; }
		void setIntensity(float intensity) { intensity_ = intensity; }

		virtual void submitLightInfo() const = 0;

	protected:
		void draw() const override final {};
		void update() override final {};

		EngineUtil::ColorRGB color_;
		float intensity_ = 1.0f;

		void onStart() override;
		void onDestruction() override;
};

#endif