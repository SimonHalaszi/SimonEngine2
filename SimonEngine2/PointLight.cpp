#include "PointLight.hpp"
#include "Renderer.hpp"

void PointLight::submitLightInfo() const {
	if (!isActive_) {
		return;
	}

	EngineRender::LightData data;
	data.type_ = EngineRender::LightType::Point;
	data.color_ = color_;
	data.intensity_ = intensity_;
	data.position_ = getWorldTransform().position_;
	data.range_ = range_;

	Renderer::getInstance().submitLight(data);
}