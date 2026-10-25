#include "StressTestScene.hpp"

// Objects
#include "Robot.hpp"
#include "AxisDisplay.hpp"
#include "CameraMan.hpp"
#include "Plane.hpp"

void StressTestScene::init() {
    addRootObject(std::make_unique<Plane>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, -2.5f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(50.0f, 0.0f, 50.0f)
        ),
        EngineUtil::ColorRGB(255, 255, 255)
    ));

    // I wanted to learn the non srand, rand, time way
    // std::random_device rd;
    // std::mt19937 gen(rd());
    std::mt19937 gen(4);
    std::uniform_real_distribution<float> randomPos(-25.0f, 25.0f);
    std::uniform_real_distribution<float> randomRotation(0.0f, 360.0f);
    std::uniform_int_distribution<int> randomColor(0, 255);

    for (int i = 0; i < 3000; ++i) {
        addRootObject(std::make_unique<Robot>(
            EngineMath::Transform(
                EngineMath::Vector3(randomPos(gen), 0.0f, randomPos(gen)),
                EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(randomRotation(gen))),
                EngineMath::Vector3(1.0f, 1.0f, 1.0f)
            ),
            EngineUtil::ColorRGB(randomColor(gen), randomColor(gen), randomColor(gen))
        ));
    }

    addRootObject(std::make_unique<AxisDisplay>());

    addRootObject(std::make_unique<CameraMan>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 1.0f, 5.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        )
    ));
}

#include "Engine.hpp"

void StressTestScene::update() {
    if (InputManager::getInstance().isPressed(27)) {
        Engine::getInstance().safeExit();
    }
}

void StressTestScene::draw() const {

}

void StressTestScene::deInit() {

}