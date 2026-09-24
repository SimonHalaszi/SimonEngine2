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
            EngineMath::Vector3(0.25f, 0.25f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 0.5f, 0.5f, 1.0f }
    ));
}

void BasicScene::draw() const {

}

void BasicScene::update() {
    if (InputManager::getInstance().isPressed('p')) {
        std::cout << "Going to Perspective" << std::endl;
        setProjectionPerspective(60.0, 0.1, 100.0);
    }

    if (InputManager::getInstance().isPressed('o')) {
        std::cout << "Going to Orpho" << std::endl;
        setProjectionOrtho(-2.0, 2.0, -2.0, 2.0, -1.0, 10.0);
    }
}