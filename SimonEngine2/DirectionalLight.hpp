#ifndef DIRECTIONAL_LIGHT_HPP
#define DIRECTIONAL_LIGHT_HPP

#include "Light.hpp"

class DirectionalLight : public Light {
	public:
		DirectionalLight() = default;

		DirectionalLight(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color = {}, float intensity = 1.0f)
			: Light(localTransform, color, intensity) {}

		void submitLightInfo() const override;
};

#endif