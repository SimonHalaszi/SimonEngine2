#include "SpotLight.hpp"
#include "Renderer.hpp"

#include <cmath>
#include <stdexcept>

SpotLight::SpotLight(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	float intensity,
	float range,
	float innerAngleDegrees,
	float outerAngleDegrees
)
	: Light(localTransform, color, intensity), range_(range) {
	if (range_ <= 0.0f) {
		throw std::invalid_argument("SpotLight: range must be positive");
	}

	if (!(0.0f < innerAngleDegrees && innerAngleDegrees < outerAngleDegrees && outerAngleDegrees < 90.0f)) {
		throw std::invalid_argument("SpotLight: cone angles must satisfy 0 < inner < outer < 90 degrees");
	}

	innerConeCos_ = std::cos(EngineMath::degreesToRadians(innerAngleDegrees));
	outerConeCos_ = std::cos(EngineMath::degreesToRadians(outerAngleDegrees));

	if (innerConeCos_ <= outerConeCos_) {
		throw std::invalid_argument("SpotLight: cone angles are too close to distinguish");
	}
}

void SpotLight::submitLightInfo() const {
	const auto& worldTransform = getWorldTransform();

	EngineRender::LightData data;
	data.type_ = EngineRender::LightType::Spot;
	data.color_ = color_;
	data.intensity_ = intensity_;
	data.position_ = worldTransform.position_;
	data.direction_ = worldTransform.forward().normalized();
	data.range_ = range_;
	data.innerConeCos_ = innerConeCos_;
	data.outerConeCos_ = outerConeCos_;

	Renderer::getInstance().submitLight(data);
}