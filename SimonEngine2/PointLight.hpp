#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include "Light.hpp"

class PointLight : public Light {
	public:
		PointLight() = default;
		PointLight(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, float intensity, float range)
			: Light(localTransform, color, intensity), range_(range) {}

		void submitLightInfo() const override;

	private:
		float range_ = 10.0f;
};

#endif