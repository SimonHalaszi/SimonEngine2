#include "TestScene.hpp"

#include "ProperPrim.hpp"

void TestScene::init() {
	camera_.setPerspective(60.0, 0.1, 100.0);
	camera_.setPosition({ 0.0f, 0.0f, 5.0f });
	camera_.setTarget({ 0.0f, 0.0f, 0.0f });

	addRootObject(std::make_unique<ProperPrim>(
		EngineMath::Transform(
			EngineMath::Vector3(0.0f, 0.0f, 0.0f),
			EngineMath::Quaternion::identity(),
			EngineMath::Vector3(1.0f, 1.0f, 1.0f)
		),
		EngineUtil::ColorRGB(125, 125, 0),
		"../Shaders/Basic.vert",
		"../Shaders/Color.frag"
	));
}

#include "Engine.hpp"

void TestScene::update() {
	if (InputManager::getInstance().isPressed(27)) {
		Engine::getInstance().safeExit();
	}
}

void TestScene::draw() const {

}

void TestScene::deInit() {

}