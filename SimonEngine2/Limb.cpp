#include "Limb.hpp"

#include <memory>
#include <utility>

#include "EngineMath.hpp"

#include "MaterialRegistry.hpp"

#include "Primitive.hpp"
#include "RectangularPrism.hpp"

Limb::Limb(const EngineMath::Transform& localTransform, const EngineRender::Material* material, const EngineMath::Vector3& size) {
    localTransform_ = localTransform;
    material_ = material;
    size_ = size;
}

void Limb::onStart() {
    if (material_ == nullptr) {
        material_ = MaterialRegistry::getInstance().getOrCreate("null", EngineRender::Material());
    }

    std::unique_ptr<Object> highJoint = std::make_unique<Object>();
    highJoint->setPosition(EngineMath::Vector3(0.0f, size_.y_ * 0.5f, 0.0f));
    highJoint_ = highJoint.get();
    addChild(std::move(highJoint));

    const EngineMath::Vector3 halfSize(size_.x_, size_.y_ * 0.5f, size_.z_);
    const float quarterY = size_.y_ * 0.25f;

    std::unique_ptr<RectangularPrism> upperMesh = std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, -quarterY, 0.0f),
            EngineMath::Quaternion::identity(),
            halfSize
        ),
        material_
    );
    upperHalf_ = upperMesh.get();
    highJoint_->addChild(std::move(upperMesh));

    std::unique_ptr<Object> middleJoint = std::make_unique<Object>();
    middleJoint->setPosition(EngineMath::Vector3(0.0f, -size_.y_ * 0.5f, 0.0f));
    middleJoint_ = middleJoint.get();
    highJoint_->addChild(std::move(middleJoint));

    std::unique_ptr<RectangularPrism> lowerMesh = std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, -quarterY, 0.0f),
            EngineMath::Quaternion::identity(),
            halfSize
        ),
        material_
    );
    lowerHalf_ = lowerMesh.get();
    middleJoint_->addChild(std::move(lowerMesh));
}

void Limb::pivotHigh(float radians) {
    if (!highJoint_) {
        return;
    }

    highAngle_ += radians;
    highJoint_->setRotation(
        EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(1.0f, 0.0f, 0.0f), highAngle_)
    );
}

void Limb::setHighAngle(float radians) {
    pivotHigh(radians - highAngle_);
}

void Limb::pivotMiddle(float radians) {
    if (!middleJoint_) {
        return;
    }

    middleAngle_ += radians;
    middleJoint_->setRotation(
        EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(1.0f, 0.0f, 0.0f), middleAngle_)
    );
}

void Limb::setMiddleAngle(float radians) {
    pivotMiddle(radians - middleAngle_);
}