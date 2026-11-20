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
    const EngineRender::Material* red = MaterialRegistry::getInstance().getOrCreate("Red", EngineRender::Material(EngineUtil::ColorRGB(255, 0, 0)));
    const EngineRender::Material* green = MaterialRegistry::getInstance().getOrCreate("Green", EngineRender::Material(EngineUtil::ColorRGB(0, 255, 0)));
    const EngineRender::Material* blue = MaterialRegistry::getInstance().getOrCreate("Blue", EngineRender::Material(EngineUtil::ColorRGB(0, 0, 255)));


    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.5f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        red
    ));
    auto obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(0.5f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        red,
        8
    ));

    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.5f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(90.0f)),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        green
    ));
    obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(0.5f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        green,
        8
    ));

    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.5f),
            EngineMath::Quaternion::fromXYZ(0.0f, EngineMath::degreesToRadians(-90.0f), 0.0f),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        blue
    ));
    obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(0.5f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        blue,
        8
    ));
}

void AxisDisplay::update() {
    if (InputManager::getInstance().isPressed('3')) {
        toggleDrawing();
    }
}