#include "AxisDisplay.hpp"

#include "Engine.hpp"
#include "Line.hpp"
#include "Cone.hpp"

AxisDisplay::AxisDisplay(const EngineMath::Transform& localTransform) {
    localTransform_ = localTransform;
}

void AxisDisplay::onStart() {
    addChild(std::make_unique<Line>(
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
            EngineMath::Vector3(0.25f, 0.25f, 0.25f)
        ),
        EngineUtil::ColorRGB{ 255, 0, 0 },
        4
    ));

    addChild(std::make_unique<Line>(
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
            EngineMath::Vector3(0.25f, 0.25f, 0.25f)
        ),
        EngineUtil::ColorRGB{ 0, 255, 0 },
        4
    ));

    addChild(std::make_unique<Line>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, EngineMath::degreesToRadians(90.0f), 0.0f),
            EngineMath::Vector3(1.5f, 1.5f, 1.5f)
        ),
        EngineUtil::ColorRGB{ 0, 0, 255 }
    ));
    obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(1.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.25f, 0.25f, 0.25f)
        ),
        EngineUtil::ColorRGB{ 0, 0, 255 },
        4
    ));
}

void AxisDisplay::update() {
    if (InputManager::getInstance().isPressed('a')) {
        toggleDrawing();
    }
}