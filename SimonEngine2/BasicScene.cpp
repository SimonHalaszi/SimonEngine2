#include "BasicScene.hpp"

#include "MagicCube.hpp"
#include "AxisDisplay.hpp"

void BasicScene::init() {
    addRootObject(std::make_unique<MagicCube>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        )
    ));
    addRootObject(std::make_unique<AxisDisplay>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        )
    ));

    std::unique_ptr<Object>& first = rootObjects_.front();

    camera_.setTarget(first->getWorldTransform().position_);
    camera_.setPosition(EngineMath::Vector3(5.0f, 5.0f, 5.0f));
}

void BasicScene::draw() const {

}

void BasicScene::update() {
    if (InputManager::getInstance().isPressed('p')) {
        camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
    }

    if (InputManager::getInstance().isPressed('o')) {
        camera_.setProjectionMode(Camera::ProjectionMode::Ortho);
    }

    const bool isPerspective = (camera_.getProjectionMode() == Camera::ProjectionMode::Perspective);
    const double zoomAmount = isPerspective ? 2.0 : 0.05;

    if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_SCROLLUP)) {
        camera_.zoomIn(zoomAmount);
    }

    if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_SCROLLDOWN)) {
        camera_.zoomOut(zoomAmount);
    }

    if (InputManager::getInstance().isPressed('c')) {
        isDrawing_ = !isDrawing_;
    }

    if (InputManager::getInstance().isPressed('k')) {
        camera_.setFOV(120.0);
    }

    if (InputManager::getInstance().isPressed('l')) {
        camera_.setFOV(60.0);
    }
}