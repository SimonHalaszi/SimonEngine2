#include "BasicScene.hpp"

#include "RectangleObject.hpp"

void BasicScene::init() {
    addRootObject(std::make_unique<RectangleObject>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, -4.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.5f, 1.5f, 1.5f)
        ),
        EngineUtil::ColorRGB{ 1.0f, 0.5f, 0.5f }
    ));

    auto& first = rootObjects_.front();
    first->addChild(std::make_unique<RectangleObject>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, -1.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 0.5f, 1.0f, 0.5f }
    ));
    first->addChild(std::make_unique<RectangleObject>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 1.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 0.5f, 0.5f, 1.0f }
    ));
}

void BasicScene::draw() const {

}

void BasicScene::update() {
    if (InputManager::getInstance().isPressed('p')) {
        std::cout << "Going to Perspective" << std::endl;
        camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
    }

    if (InputManager::getInstance().isPressed('o')) {
        std::cout << "Going to Orpho" << std::endl;
        camera_.setProjectionMode(Camera::ProjectionMode::Ortho);
    }

    if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_SCROLLUP)) {
        camera_.zoomIn(0.05);
    }

    if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_SCROLLDOWN)) {
        camera_.zoomOut(0.05);
    }

    if (InputManager::getInstance().isPressed('c')) {
        isDrawing_ = !isDrawing_;
    }
}