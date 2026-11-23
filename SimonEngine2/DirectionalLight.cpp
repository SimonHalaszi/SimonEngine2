#include "DirectionalLight.hpp"

void DirectionalLight::submitLightInfo() const {
	if (!isActive_) {
		return;
	}

	EngineRender::LightData data;
	data.type_ = EngineRender::LightType::Directional;
	data.color_ = color_;
	data.intensity_ = intensity_;
	data.direction_ = getWorldTransform().forward().normalized();

	Renderer::getInstance().submitLight(data);
}