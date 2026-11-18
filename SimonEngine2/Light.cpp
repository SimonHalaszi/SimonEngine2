#include "Light.hpp"

#include "Engine.hpp"

void Light::onStart() {
	Engine::getInstance().getCurrentScene()->addLight(this);
}

void Light::onDestruction() {
	Engine::getInstance().getCurrentScene()->removeLight(this);
}