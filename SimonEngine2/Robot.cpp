#include "Robot.hpp"

#include <cmath>

#include "Engine.hpp"

#include "Limb.hpp"
#include "RectangularPrism.hpp"
#include "Ellipsoid.hpp"

#include "MaterialRegistry.hpp"

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

Robot::Robot(const EngineMath::Transform& localTransform, 
    const EngineRender::Material* skin,
    const EngineRender::Material* shirt,
    const EngineRender::Material* pants
) {
    if (skin == nullptr) {
        skin = MaterialRegistry::getInstance().getOrCreate("null", EngineRender::Material());
    }
    if (shirt == nullptr) {
        shirt = MaterialRegistry::getInstance().getOrCreate("null", EngineRender::Material());
    }
    if (pants == nullptr) {
        pants = MaterialRegistry::getInstance().getOrCreate("null", EngineRender::Material());
    }

    skin_ = skin;
    shirt_ = shirt;
    pants_ = pants;

    isAnimating_ = true;
    localTransform_ = localTransform;
    rotationStep_ = 5.0f;
}

void Robot::onStart() {
    // Body
    addChild(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(torsoWide, torsoTall, torsoThick)
        ),
        shirt_
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
            shirt_,
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
            shirt_,
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
            pants_,
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
            pants_,
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
        skin_,
        12,
        12
    ));
}

void Robot::update() {
    if (InputManager::getInstance().isPressed('a')) {
        toggleAnimation();
    }

    if (isAnimating_) {
        constexpr float cyclesPerSecond = 1.5f;
        const float maxSwing = EngineMath::degreesToRadians(45.0f);
        const float maxBend = EngineMath::degreesToRadians(45.0f);

        const float phase = Engine::getInstance().getTime() * 2.0f * EngineMath::PI * cyclesPerSecond;
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