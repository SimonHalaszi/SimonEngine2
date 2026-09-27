#include "Robot.hpp"

#include "Engine.hpp"
#include "RectangularPrism.hpp"
#include "Sphere.hpp"

namespace {
    constexpr float torsoWide = 1.0f;
    constexpr float torsoTall = 1.5f;
    constexpr float torsoThick = 0.5f;

    constexpr float armWideThick = torsoThick;
    constexpr float armTall = torsoTall;

    constexpr float legWideThick = torsoThick;
    constexpr float legTall = torsoTall;

    constexpr float headDiameter = torsoWide;
}

Robot::Robot(const EngineMath::Transform& localTransform, EngineUtil::ColorRGB color) {
    localTransform_ = localTransform;
    color_ = color;
}

void Robot::onStart() {
    // Body
    addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(torsoWide, torsoTall, torsoThick)
        ),
        color_
    ));

    // Left Arm
    EngineMath::Transform leftArmTransform = EngineMath::Transform(
        EngineMath::Vector3(0.0f - ((torsoWide + armWideThick) / 2.0f), 0.0f, 0.0f),
        EngineMath::Quaternion::identity(),
        EngineMath::Vector3(armWideThick, armTall, armWideThick)
    );

    EngineMath::Vector3 leftArmPivot_ = leftArmTransform.position_ + EngineMath::Vector3(0.0f + (armWideThick / 2.0f), (armTall / 2.0f), 0.0f);

    leftArmTransform.rotateAround(leftArmPivot_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(-35.0f));

    addChild(std::make_unique<RectangularPrism>(
        leftArmTransform,
        color_
    ));

    // Right Arm
    EngineMath::Transform rightArmTransform = EngineMath::Transform(
        EngineMath::Vector3(0.0f + ((torsoWide + armWideThick) / 2.0f), 0.0f, 0.0f),
        EngineMath::Quaternion::identity(),
        EngineMath::Vector3(armWideThick, armTall, armWideThick)
    );

    EngineMath::Vector3 rightArmPivot_ = rightArmTransform.position_ + EngineMath::Vector3(0.0f - (armWideThick / 2.0f), armTall / 2.0f, 0.0f);

    rightArmTransform.rotateAround(rightArmPivot_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(35.0f));

    addChild(std::make_unique<RectangularPrism>(
        rightArmTransform,
        color_
    ));

    // Left Leg
    addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f - (torsoWide / 4.0f), 0.0f - ((torsoTall + legTall) / 2.0f), 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(legWideThick, legTall, legWideThick)
        ),
        color_
    ));
    // Right Leg
    addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f + (torsoWide / 4.0f), 0.0f - ((torsoTall + legTall) / 2.0f), 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(legWideThick, legTall, legWideThick)
        ),
        color_
    ));
    
    // Head
    addChild(std::make_unique<Sphere>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f + ((torsoTall + headDiameter) / 2.0f), 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(headDiameter, headDiameter, headDiameter)
        ),
        color_,
        16,
        16
    ));
}

void Robot::update() {
    if (InputManager::getInstance().isPressed('m')) {
        toggleDrawing();
    }
}

void Robot::changeColor(EngineUtil::ColorRGB color) {
    for (auto& obj : children_) {
        Primitive* primitive = dynamic_cast<Primitive*>(obj.get());
        if (primitive) {
            primitive->setColor(color);
        }
    }
}