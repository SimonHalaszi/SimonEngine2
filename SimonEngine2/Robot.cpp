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
            EngineMath::Vector3(armWideThick, armTall, armWideThick)
        );

        EngineMath::Vector3 pivot = t.position_ + EngineMath::Vector3(0.0f, armTall / 2.0f, 0.0f);
        t.rotateAround(pivot, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(-35.0f));

        std::unique_ptr<Limb> limb = std::make_unique<Limb>(t, color_);
        leftArm_ = limb.get();
        addChild(std::move(limb));
    }

    // Right Arm (Limb)
    {
        EngineMath::Transform t(
            EngineMath::Vector3((torsoWide + armWideThick) / 2.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(armWideThick, armTall, armWideThick)
        );

        EngineMath::Vector3 pivot = t.position_ + EngineMath::Vector3(0.0f, armTall / 2.0f, 0.0f);
        t.rotateAround(pivot, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(35.0f));

        std::unique_ptr<Limb> limb = std::make_unique<Limb>(t, color_);
        rightArm_ = limb.get();
        addChild(std::move(limb));
    }

    // Left Leg (Limb)
    {
        EngineMath::Transform t(
            EngineMath::Vector3(-torsoWide / 4.0f, -(torsoTall + legTall) / 2.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(legWideThick, legTall, legWideThick)
        );

        std::unique_ptr<Limb> limb = std::make_unique<Limb>(t, color_);
        leftLeg_ = limb.get();
        addChild(std::move(limb));
    }

    // Right Leg (Limb)
    {
        EngineMath::Transform t(
            EngineMath::Vector3(torsoWide / 4.0f, -(torsoTall + legTall) / 2.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(legWideThick, legTall, legWideThick)
        );

        std::unique_ptr<Limb> limb = std::make_unique<Limb>(t, color_);
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
            leftArm_->pivotMiddle(EngineMath::degreesToRadians(5.0f));
        }
        if (rightArm_) {
            rightArm_->pivotMiddle(EngineMath::degreesToRadians(5.0f));
        }
    }
    if (InputManager::getInstance().isSpecialKeyPressed(GLUT_KEY_F4)) {
        toggleDrawing();
    }
}

void Robot::setColor(EngineUtil::ColorRGB color) {
    color_ = color;

    // Since the Homework 2 refactor the arms and legs are Limbs (an Object holding two
    // prisms), not Primitives, so the old Primitive-only loop silently skipped all four
    // of them and you got a red torso with blue noodle arms. Paint the limbs too.
    for (auto& obj : children_) {
        Primitive* primitive = dynamic_cast<Primitive*>(obj.get());
        if (primitive) {
            primitive->setColor(color);
            continue;
        }

        Limb* limb = dynamic_cast<Limb*>(obj.get());
        if (limb) {
            limb->setColor(color);
        }
    }
}