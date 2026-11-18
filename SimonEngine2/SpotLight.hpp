#ifndef SPOT_LIGHT_HPP
#define SPOT_LIGHT_HPP

#include "Light.hpp"

class SpotLight : public Light {
	public:
		SpotLight(
			const EngineMath::Transform& localTransform,
			const EngineUtil::ColorRGB& color = {},
			float intensity = 1.0f,
			float range = 10.0f,
			float innerAngleDegrees = 15.0f,
			float outerAngleDegrees = 25.0f
		);

		void submitLightInfo() const override;

	private:
		float range_;
		float innerConeCos_;
		float outerConeCos_;
};

#endif