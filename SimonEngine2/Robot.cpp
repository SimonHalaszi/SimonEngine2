#include "Robot.hpp"

#include "Engine.hpp"

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
    rotationStep = 5.0f;
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

    // Left Arm (Limb)
    {
        EngineMath::Transform t(
            EngineMath::Vector3(-(torsoWide + armWideThick) / 2.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        );

        std::unique_ptr<Limb> limb = std::make_unique<Limb>(
            t,
            color_,
            EngineMath::Vector3(armWideThick, armTall, armWideThick)
        );
        leftArm_ = limb.get();
        addChild(std::move(limb));
    }

    // Right Arm (Limb)
    {
        EngineMath::Transform t(
            EngineMath::Vector3((torsoWide + armWideThick) / 2.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        );

        std::unique_ptr<Limb> limb = std::make_unique<Limb>(
            t,
            color_,
            EngineMath::Vector3(armWideThick, armTall, armWideThick)
        );
        rightArm_ = limb.get();
        addChild(std::move(limb));
    }

    // Left Leg (Limb)
    {
        EngineMath::Transform t(
            EngineMath::Vector3(-torsoWide / 4.0f, -(torsoTall + legTall) / 2.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        );

        std::unique_ptr<Limb> limb = std::make_unique<Limb>(
            t,
            color_,
            EngineMath::Vector3(legWideThick, legTall, legWideThick)
        );
        leftLeg_ = limb.get();
        addChild(std::move(limb));
    }

    // Right Leg (Limb)
    {
        EngineMath::Transform t(
            EngineMath::Vector3(torsoWide / 4.0f, -(torsoTall + legTall) / 2.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        );

        std::unique_ptr<Limb> limb = std::make_unique<Limb>(
            t,
            color_,
            EngineMath::Vector3(legWideThick, legTall, legWideThick)
        );
        rightLeg_ = limb.get();
        addChild(std::move(limb));
    }

    // Head
    addChild(std::make_unique<Sphere>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, (torsoTall + headDiameter) / 2.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(headDiameter, headDiameter, headDiameter)
        ),
        color_,
        16,
        16
    ));
}

void Robot::update() {
    if (InputManager::getInstance().isPressed('4')) {
        if (leftArm_) {
            leftArm_->pivotMiddle(EngineMath::degreesToRadians(rotationStep));
        }
        if (rightArm_) {
            rightArm_->pivotMiddle(EngineMath::degreesToRadians(rotationStep));
        }
    }
    if (InputManager::getInstance().isSpecialKeyPressed(mapSpecialKey(GLUT_KEY_F4))) {
        if (leftArm_) {
            leftArm_->pivotMiddle(EngineMath::degreesToRadians(-rotationStep));
        }
        if (rightArm_) {
            rightArm_->pivotMiddle(EngineMath::degreesToRadians(-rotationStep));
        }
    }

    if (InputManager::getInstance().isPressed('5')) {
        if (leftArm_) {
            leftArm_->pivotHigh(EngineMath::degreesToRadians(rotationStep));
        }
        if (rightArm_) {
            rightArm_->pivotHigh(EngineMath::degreesToRadians(rotationStep));
        }
    }
    if (InputManager::getInstance().isSpecialKeyPressed(mapSpecialKey(GLUT_KEY_F5))) {
        if (leftArm_) {
            leftArm_->pivotHigh(EngineMath::degreesToRadians(-rotationStep));
        }
        if (rightArm_) {
            rightArm_->pivotHigh(EngineMath::degreesToRadians(-rotationStep));
        }
    }

    if (InputManager::getInstance().isPressed('7')) {
        if (leftLeg_) {
            leftLeg_->pivotMiddle(EngineMath::degreesToRadians(rotationStep));
        }
        if (rightLeg_) {
            rightLeg_->pivotMiddle(EngineMath::degreesToRadians(rotationStep));
        }
    }
    if (InputManager::getInstance().isSpecialKeyPressed(mapSpecialKey(GLUT_KEY_F7))) {
        if (leftLeg_) {
            leftLeg_->pivotMiddle(EngineMath::degreesToRadians(-rotationStep));
        }
        if (rightLeg_) {
            rightLeg_->pivotMiddle(EngineMath::degreesToRadians(-rotationStep));
        }
    }

    if (InputManager::getInstance().isPressed('6')) {
        if (leftLeg_) {
            leftLeg_->pivotHigh(EngineMath::degreesToRadians(rotationStep));
        }
        if (rightLeg_) {
            rightLeg_->pivotHigh(EngineMath::degreesToRadians(rotationStep));
        }
    }
    if (InputManager::getInstance().isSpecialKeyPressed(mapSpecialKey(GLUT_KEY_F6))) {
        if (leftLeg_) {
            leftLeg_->pivotHigh(EngineMath::degreesToRadians(-rotationStep));
        }
        if (rightLeg_) {
            rightLeg_->pivotHigh(EngineMath::degreesToRadians(-rotationStep));
        }
    }
}

void Robot::setColor(EngineUtil::ColorRGB color) {
    for (auto& obj : children_) {
        Primitive* primitive = dynamic_cast<Primitive*>(obj.get());
        if (primitive) {
            primitive->setColor(color);
        }
    }
}