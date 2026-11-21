#include "Skybox.hpp"
#include "Engine.hpp"

void Skybox::onStart() {
	Engine::getInstance().getCurrentScene()->addSkybox(this);
}

void Skybox::onDestruction() {
	Engine::getInstance().getCurrentScene()->removeSkybox(this);
}