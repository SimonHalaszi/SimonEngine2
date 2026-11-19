#include "Robot.hpp"

#include <cmath>

#include "Engine.hpp"

#include "Limb.hpp"
#include "RectangularPrism.hpp"
#include "Ellipsoid.hpp"

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
    isAnimating_ = true;
    localTransform_ = localTransform;
    color_ = color;
    rotationStep_ = 5.0f;
}

void Robot::onStart() {
    EngineRender::Material material = EngineRender::Material(
        color_,
        0.5f,
        0.0f,
        "../Shaders/Lit.vert",
        "../Shaders/Lit.frag"
    );

    // Body
    addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(torsoWide, torsoTall, torsoThick)
        ),
        material
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
    addChild(std::make_unique<Ellipsoid>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, (torsoTall + headDiameter) / 2.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(headDiameter, headDiameter, headDiameter)
        ),
        material,
        12,
        12
    ));
}

void Robot::update() {
    const Scene* scene = Engine::getInstance().getCurrentScene();
    if (!scene) {
        return;
    }

    if (InputManager::getInstance().isPressed('a')) {
        toggleAnimation();
    }

    if (isAnimating_) {
        constexpr int framesPerCycle = 12;
        const float maxSwing = EngineMath::degreesToRadians(45.0f);
        const float maxBend = EngineMath::degreesToRadians(45.0f);

        const float phase = 2.0f * EngineMath::PI * (scene->getAnimationFrame() % framesPerCycle) / framesPerCycle;
        const float swing = std::sin(phase);

        const float leftBend = maxBend * (swing > 0.0f ? swing : 0.0f);
        const float rightBend = maxBend * (swing < 0.0f ? -swing : 0.0f);

        if (leftArm_) {
            leftArm_->setHighAngle(maxSwing * swing);
            leftArm_->setMiddleAngle(leftBend);
        }

        if (rightLeg_) {
            rightLeg_->setHighAngle(maxSwing * swing);
            rightLeg_->setMiddleAngle(-leftBend);
        }

        if (rightArm_) {
            rightArm_->setHighAngle(-maxSwing * swing);
            rightArm_->setMiddleAngle(rightBend);
        }

        if (leftLeg_) {
            leftLeg_->setHighAngle(-maxSwing * swing);
            leftLeg_->setMiddleAngle(-rightBend);
        }
    }
}