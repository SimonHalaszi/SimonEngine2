#include "MagicCube.hpp"

#include "Engine.hpp"
#include "RectangularPrism.hpp"
#include "Sphere.hpp"

MagicCube::MagicCube(const EngineMath::Transform& localTransform) {
    localTransform_ = localTransform;
    int updateSpeed = Engine::getInstance().getCurrentScene()->getUpdateSpeed();
    rotationSpeed_ = EngineMath::degreesToRadians(90.0f / updateSpeed);
}

void MagicCube::onStart() {
    addChild(std::make_unique<Sphere>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        EngineUtil::ColorRGB{ 102, 102, 102 },
        16,
        16
    ));

    std::unique_ptr<Object>& first = children_.front();

    first->addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, -1.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 255, 0, 0 }
    ));
    first->addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 1.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 255, 115, 0 }
    ));
    first->addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 1.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 255, 255, 0 }
    ));
    first->addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, -1.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 255, 255, 255 }
    ));
    first->addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(1.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 0, 60, 255 }
    ));
    first->addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(-1.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB{ 26, 255, 0 }
    ));
}

void MagicCube::update() {
    rotationRadians_ += rotationSpeed_;

    setRotation(
        EngineMath::Quaternion::fromXYZ(
            rotationRadians_ * 0.75f,
            rotationRadians_,
            rotationRadians_ * 0.25f
        )
    );
    if (InputManager::getInstance().isPressed('m')) {
        toggleDrawing();
    }
}