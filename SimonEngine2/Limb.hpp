#ifndef LIMB_HPP
#define LIMB_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"
#include "Primitive.hpp"

class Limb : public Object {
public:
    Limb() = default;
    Limb(
        const EngineMath::Transform& localTransform,
        EngineUtil::ColorRGB color,
        const EngineMath::Vector3& size
    );

    void onStart() override final;
    void update() override final {}

    void setColor(EngineUtil::ColorRGB color);

    // Shoulder / Pelvis joint
    void pivotHigh(float radians);

    // Elbow / Knee joint
    void pivotMiddle(float radians);

private:
    Object* highJoint_ = nullptr;
    Object* middleJoint_ = nullptr;

    Primitive* upperHalf_ = nullptr;
    Primitive* lowerHalf_ = nullptr;

    EngineMath::Vector3 size_ = { 1.0f, 1.0f, 1.0f };
    EngineUtil::ColorRGB color_;

    float highAngle_ = 0.0f;
    float middleAngle_ = 0.0f;
};

#endif