#include "BasicScene.hpp"

#include "Rectangle.hpp"

void BasicScene::init() {
    addRootObject(std::make_unique<Rectangle>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, -4.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.5f, 1.5f, 1.5f)
        ),
        EngineUtil::ColorRGB{ 1.0f, 0.5f, 0.5f }
    ));

    auto& first = rootObjects_.front();
    first->addChild(std::make_unique<Rectangle>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, -1.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 0.5f, 1.0f, 0.5f }
    ));
    first->addChild(std::make_unique<Rectangle>(
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

}