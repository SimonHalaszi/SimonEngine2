#include "TestScene.hpp"

#include "Ellipsoid.hpp"

void TestScene::init() {
	camera_.setPerspective(60.0, 0.1, 100.0);
	camera_.setPosition({ 0.0f, 0.0f, 5.0f });
	camera_.setTarget({ 0.0f, 0.0f, 0.0f });

	addRootObject(std::make_unique<Ellipsoid>(
		EngineMath::Transform(
			EngineMath::Vector3(0.0f, 0.0f, 0.0f),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(1.0f, 1.0f, 1.0f)
		),
		EngineUtil::ColorRGB(125, 125, 0),
		12,
		12
	));
}

#include "Engine.hpp"

void TestScene::update() {
	if (InputManager::getInstance().isPressed(27)) {
		Engine::getInstance().safeExit();
	}
	if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_SCROLLUP)) {
		camera_.zoomIn(1.0);
	}
	if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_SCROLLDOWN)) {
		camera_.zoomOut(1.0);
	}
}

void TestScene::draw() const {

}

void TestScene::deInit() {

}