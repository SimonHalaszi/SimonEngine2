#include "MagicCubeScene.hpp"

void MagicCubeScene::init() {
    addRootObject(std::make_unique<MagicCube>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        )
    ));
    addRootObject(std::make_unique<AxisDisplay>());

    std::unique_ptr<Object>& first = rootObjects_.front();

    camera_.setTarget(first->getWorldTransform().position_);
    camera_.setPosition(EngineMath::Vector3(5.0f, 5.0f, 5.0f));

    magicCube_ = getMagicCube();
}

#include "Engine.hpp"

void MagicCubeScene::update() {
    if (InputManager::getInstance().isPressed(27)) {
        Engine::getInstance().safeExit();
    }
}

void MagicCubeScene::deInit() {
    magicCube_ = nullptr;
}

MagicCube* MagicCubeScene::getMagicCube() {
    for (auto& obj : rootObjects_) {
        MagicCube* magicCube = dynamic_cast<MagicCube*>(obj.get());
        if (magicCube) {
            return magicCube;
        }
    }
    return nullptr;
}