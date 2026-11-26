#include "CubeMap.hpp"

#include <stdexcept>

#include "Engine.hpp"
#include "Renderer.hpp"

#include "TextureRegistry.hpp"

CubeMap::CubeMap(const EngineMath::Transform& localTransform, GLuint textureID)
	: textureID_(textureID) { 
	localTransform_ = localTransform; 
}

void CubeMap::onStart() {
	if (textureID_ == 0) {
		throw std::runtime_error("CubeMap: texture ID cannot be zero");
	}

	Engine::getInstance().getCurrentScene()->addCubeMap(this);
}

void CubeMap::onDestruction() {
	Engine::getInstance().getCurrentScene()->removeCubeMap(this);
}

void CubeMap::submitCubeMapInfo() const {
	if (!isActive_) {
		return;
	}

	EngineRender::CubeMapData data;
	data.position_ = getWorldTransform().position_;
	data.textureID_ = textureID_;

	Renderer::getInstance().submitCubeMap(data);
}