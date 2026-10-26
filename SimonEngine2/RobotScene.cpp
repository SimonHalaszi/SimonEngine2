#include "RobotScene.hpp"

// Objects
#include "Robot.hpp"
#include "AxisDisplay.hpp"
#include "CameraMan.hpp"

#include "Cone.hpp"
#include "Cylinder.hpp"
#include "Ellipsoid.hpp"
#include "Plane.hpp"
#include "RectangularPrism.hpp"
#include "ObjMesh.hpp"

void RobotScene::init() {
    addRootObject(std::make_unique<Plane>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(50.0f, 0.0f, 50.0f)
        ),
        EngineUtil::ColorRGB(255, 255, 255),
        4
    ));

    addRootObject(std::make_unique<Cone>(
        EngineMath::Transform(
            EngineMath::Vector3(-6.0f, 2.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        EngineUtil::ColorRGB(100, 140, 220),
        16,
        "../Shaders/Basic.vert",
        "../Shaders/Bounce.frag"
    ));

    addRootObject(std::make_unique<Cylinder>(
        EngineMath::Transform(
            EngineMath::Vector3(-3.0f, 2.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        EngineUtil::ColorRGB(100, 140, 220),
        16,
        "../Shaders/Basic.vert",
        "../Shaders/Bounce.frag"
    ));

    addRootObject(std::make_unique<Ellipsoid>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 2.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        EngineUtil::ColorRGB(100, 140, 220),
        16,
        16,
        "../Shaders/Basic.vert",
        "../Shaders/Bounce.frag"
    ));

    addRootObject(std::make_unique<RectangularPrism>(
        EngineMath::Transform(
            EngineMath::Vector3(3.0f, 2.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        EngineUtil::ColorRGB(100, 140, 220),
        "../Shaders/Basic.vert",
        "../Shaders/Bounce.frag"
    ));

    addRootObject(std::make_unique<ObjMesh>(
        EngineMath::Transform(
            EngineMath::Vector3(6.0f, 2.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.5f, 0.5f, 0.5f)
        ),
        EngineUtil::ColorRGB(100, 140, 220),
		"../Assets/teapot.obj",
        "../Shaders/Basic.vert",
        "../Shaders/Bounce.frag"
    ));


    // I wanted to learn the non srand, rand, time way
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> randomPos(-25.0f, 25.0f);
    std::uniform_real_distribution<float> randomRotation(0.0f, 360.0f);
    std::uniform_int_distribution<int> randomColor(0, 255);

    for (int i = 0; i < 25; ++i) {
        addRootObject(std::make_unique<Robot>(
            EngineMath::Transform(
                EngineMath::Vector3(randomPos(gen), 2.25f, randomPos(gen)),
                EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(randomRotation(gen))),
                EngineMath::Vector3(1.0f, 1.0f, 1.0f)
            ),
            EngineUtil::ColorRGB(randomColor(gen), randomColor(gen), randomColor(gen))
        ));
    }

    addRootObject(std::make_unique<AxisDisplay>());

    addRootObject(std::make_unique<CameraMan>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 3.5f, 5.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        )
    ));

    std::cout << "Controls:\n";
    std::cout << "1: Show primitive mesh as wires\n";
    std::cout << "2: Show primitive mesh as solids\n";
    std::cout << "3: Toggle axis display\n";
    std::cout << "4/F4: Increase/Decrease angle of elbows\n";
    std::cout << "5/F5: Increase/Decrease angle of shoulders\n";
    std::cout << "6/F6: Increase/Decrease angle of hips\n";
    std::cout << "7/F7: Increase/Decrease angle of knees\n";
    std::cout << "Arrow Keys: Tank controls\n";
    std::cout << "Space/LCtrl: Fly up/down\n";
    std::cout << "0: Print FPS\n";
    std::cout << std::endl;
}

#include "Engine.hpp"

void RobotScene::update() {
    if (InputManager::getInstance().isPressed(27)) {
        Engine::getInstance().safeExit();
    }
    if (InputManager::getInstance().isPressed('0')) {
        std::cout << "FPS: " << Engine::getInstance().getFPS() << std::endl;
    }
}

void RobotScene::draw() const {

}

void RobotScene::deInit() {

}