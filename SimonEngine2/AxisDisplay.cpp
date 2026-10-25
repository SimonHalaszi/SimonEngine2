#include "AxisDisplay.hpp"

#include "Engine.hpp"
#include "Edge.hpp"
#include "Cone.hpp"

AxisDisplay::AxisDisplay() {
    localTransform_ = EngineMath::Transform(
        EngineMath::Vector3(0.0f, 0.0f, 0.0f),
        EngineMath::Quaternion::identity(),
        EngineMath::Vector3(1.0f, 1.0f, 1.0f)
    );
}

void AxisDisplay::onStart() {
    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.5f, 1.5f, 1.5f)
        ),
        EngineUtil::ColorRGB{ 255, 0, 0 }
    ));
    auto obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(1.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        EngineUtil::ColorRGB{ 255, 0, 0 },
        8
    ));

    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(90.0f)),
            EngineMath::Vector3(1.5f, 1.5f, 1.5f)
        ),
        EngineUtil::ColorRGB{ 0, 255, 0 }
    ));
    obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(1.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        EngineUtil::ColorRGB{ 0, 255, 0 },
        8
    ));

    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, EngineMath::degreesToRadians(-90.0f), 0.0f),
            EngineMath::Vector3(1.5f, 1.5f, 1.5f)
        ),
        EngineUtil::ColorRGB{ 0, 0, 255 }
    ));
    obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(1.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        EngineUtil::ColorRGB{ 0, 0, 255 },
        8
    ));
}

void AxisDisplay::update() {
    if (InputManager::getInstance().isPressed('3')) {
        toggleDrawing();
    }
}