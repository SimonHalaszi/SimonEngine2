#include "Limb.hpp"
#include "RectangularPrism.hpp"

Limb::Limb(
    const EngineMath::Transform& localTransform,
    EngineUtil::ColorRGB color,
    const EngineMath::Vector3& size
) {
    localTransform_ = localTransform;
    color_ = color;
    size_ = size;
}

void Limb::onStart() {
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
        color_
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
        color_
    );
    lowerHalf_ = lowerMesh.get();
    middleJoint_->addChild(std::move(lowerMesh));
}

void Limb::setColor(EngineUtil::ColorRGB color) {
    color_ = color;
    if (upperHalf_) {
        upperHalf_->setColor(color);
    }
    if (lowerHalf_) {
        lowerHalf_->setColor(color);
    }
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

void Limb::pivotMiddle(float radians) {
    if (!middleJoint_) {
        return;
    }

    middleAngle_ += radians;
    middleJoint_->setRotation(
        EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(1.0f, 0.0f, 0.0f), middleAngle_)
    );
}