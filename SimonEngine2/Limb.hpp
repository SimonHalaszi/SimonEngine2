#ifndef LIMB_HPP
#define LIMB_HPP

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"
#include "Primitive.hpp"

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