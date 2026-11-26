#include "AxisDisplay.hpp"

#include <iterator>
#include <memory>

#include "Engine.hpp"
#include "EngineMath.hpp"
#include "EngineRender.hpp"
#include "EngineUtil.hpp"
#include "InputManager.hpp"

#include "MaterialRegistry.hpp"

#include "Cone.hpp"
#include "Edge.hpp"

AxisDisplay::AxisDisplay() {
    localTransform_ = EngineMath::Transform(
        EngineMath::Vector3(0.0f, 0.0f, 0.0f),
        EngineMath::Quaternion::identity(),
        EngineMath::Vector3(1.0f, 1.0f, 1.0f)
    );
}

void AxisDisplay::onStart() {
    MaterialRegistry::getInstance().getOrCreate("red", EngineRender::Material(EngineUtil::ColorRGB(255, 0, 0)));
    MaterialRegistry::getInstance().getOrCreate("green", EngineRender::Material(EngineUtil::ColorRGB(0, 255, 0)));
    MaterialRegistry::getInstance().getOrCreate("blue", EngineRender::Material(EngineUtil::ColorRGB(0, 0, 255)));


    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.5f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        MaterialRegistry::getInstance().get("red")
    ));
    auto obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(0.5f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        MaterialRegistry::getInstance().get("red"),
        8
    ));

    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.5f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(90.0f)),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        MaterialRegistry::getInstance().get("green")
    ));
    obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(0.5f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        MaterialRegistry::getInstance().get("green"),
        8
    ));

    addChild(std::make_unique<Edge>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.5f),
            EngineMath::Quaternion::fromXYZ(0.0f, EngineMath::degreesToRadians(-90.0f), 0.0f),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        MaterialRegistry::getInstance().get("blue")
    ));
    obj = std::prev(children_.end());
    (*obj)->addChild(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(0.5f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromXYZ(0.0f, 0.0f, EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(0.0625f, 0.0625f, 0.0625f)
        ),
        MaterialRegistry::getInstance().get("blue"),
        8
    ));
}

void AxisDisplay::update() {
    if (InputManager::getInstance().isPressed('3')) {
        toggleDrawing();
    }
}