#include "ObjScene.hpp"

void ObjScene::init() {
    addRootObject(std::make_unique<ObjMesh>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.25f, 0.25f, 0.25f)
        ),
        EngineUtil::ColorRGB(50, 185, 235),
        "../Assets/teapot.obj"
    ));
    addRootObject(std::make_unique<AxisDisplay>());

    std::unique_ptr<Object>& first = rootObjects_.front();

    camera_.setTarget(first->getWorldTransform().position_);
    camera_.setPosition(EngineMath::Vector3(5.0f, 5.0f, 5.0f));

    objMesh_ = getObjMesh();
}

#include "Engine.hpp"

void ObjScene::update() {
    if (InputManager::getInstance().isPressed(27)) {
        Engine::getInstance().safeExit();
    }
}

void ObjScene::deInit() {
    objMesh_ = nullptr;
}

ObjMesh* ObjScene::getObjMesh() {
    for (auto& obj : rootObjects_) {
        ObjMesh* objectMesh = dynamic_cast<ObjMesh*>(obj.get());
        if (objectMesh) {
            return objectMesh;
        }
    }
    return nullptr;
}