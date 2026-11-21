#include "CubeMap.hpp"
#include "Engine.hpp"
#include "TextureRegistry.hpp"

void CubeMap::onStart() {
	Engine::getInstance().getCurrentScene()->addCubeMap(this);
}

void CubeMap::onDestruction() {
	Engine::getInstance().getCurrentScene()->removeCubeMap(this);
}

void CubeMap::submitCubeMapInfo() const {
	EngineRender::CubeMapData data;
	data.position_ = getWorldTransform().position_;
	data.textureID_ = textureID_;

	Renderer::getInstance().submitCubeMap(data);
}