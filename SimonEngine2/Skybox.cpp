#include "Skybox.hpp"
#include "Engine.hpp"

void Skybox::onStart() {
	if (textureID_ == 0) {
		throw std::runtime_error("Skybox: texture ID cannot be zero");
	}

	Engine::getInstance().getCurrentScene()->addSkybox(this);
}

void Skybox::onDestruction() {
	Engine::getInstance().getCurrentScene()->removeSkybox(this);
}

void Skybox::submitSkyboxInfo() const {
	if (!isActive_) {
		return;
	}

	Renderer::getInstance().submitSkybox(textureID_);
}