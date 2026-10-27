#include "CameraMan.hpp"

CameraMan::CameraMan(const EngineMath::Transform& localTransform) {
	localTransform_ = localTransform;
	speed_ = 5.0f;
}

#include "Engine.hpp"

void CameraMan::onStart() {
	Scene* scene = Engine::getInstance().getCurrentScene();
	scene->getCamera().setPerspective(90, 0.1, 100.0);
}

void CameraMan::update() {
	Scene* scene = Engine::getInstance().getCurrentScene();

	const float deltaTime = Engine::getInstance().getDeltaTime();

	EngineMath::Vector3 pos = getLocalTransform().position_;
	EngineMath::Quaternion rot = getLocalTransform().rotation_;

	const EngineMath::Vector3 forward = rot.rotate(EngineMath::Vector3(0.0f, 0.0f, -1.0f));
	const float moveStep = speed_ * deltaTime;

	if (InputManager::getInstance().isSpecialKeyDown(mapSpecialKey(GLUT_KEY_UP))) {
		pos += forward * moveStep;
	}
	if (InputManager::getInstance().isSpecialKeyDown(mapSpecialKey(GLUT_KEY_DOWN))) {
		pos -= forward * moveStep;
	}

	if (InputManager::getInstance().isSpecialKeyDown(mapSpecialKey(GLUT_KEY_CTRL_L))) {
		pos.y_ -= moveStep;
	}
	if (InputManager::getInstance().isDown(' ')) {
		pos.y_ += moveStep;
	}

	float yawDelta = 0.0f;
	if (InputManager::getInstance().isSpecialKeyDown(mapSpecialKey(GLUT_KEY_LEFT))) {
		yawDelta += 1.0f;
	}
	if (InputManager::getInstance().isSpecialKeyDown(mapSpecialKey(GLUT_KEY_RIGHT))) {
		yawDelta -= 1.0f;
	}

	if (yawDelta != 0.0f) {
		const float turnSpeed = EngineMath::degreesToRadians(120.0f);
		EngineMath::Quaternion yawQ = EngineMath::Quaternion::fromAxisAngle(
			EngineMath::Vector3(0.0f, 1.0f, 0.0f),
			yawDelta * turnSpeed * deltaTime
		);
		rot = (yawQ * rot).normalized();
	}

	setPosition(pos);
	setRotation(rot);

	updateWorldTransform();

	scene->getCamera().setTransform(getWorldTransform());
}