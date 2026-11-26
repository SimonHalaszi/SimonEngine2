#ifndef LIMB_HPP
#define LIMB_HPP

#include "EngineMath.hpp"
#include "EngineRender.hpp"

#include "Object.hpp"

class Primitive;

class Limb : public Object {
    public:
        Limb(
            const EngineMath::Transform& localTransform,
            const EngineRender::Material* material,
            const EngineMath::Vector3& size
        );

        void onStart() override final;
        void update() override final {}

        // Shoulder / Pelvis joint
        void pivotHigh(float radians);
        void setHighAngle(float radians);
        // Elbow / Knee joint
        void pivotMiddle(float radians);
        void setMiddleAngle(float radians);

    private:
        Object* highJoint_ = nullptr;
        Object* middleJoint_ = nullptr;

        Primitive* upperHalf_ = nullptr;
        Primitive* lowerHalf_ = nullptr;

        EngineMath::Vector3 size_ = { 1.0f, 1.0f, 1.0f };
        const EngineRender::Material* material_ = nullptr;

        float highAngle_ = 0.0f;
        float middleAngle_ = 0.0f;
};

#endif