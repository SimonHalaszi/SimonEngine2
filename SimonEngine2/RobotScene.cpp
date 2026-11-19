#include "RobotScene.hpp"

// Objects
#include "Robot.hpp"
#include "AxisDisplay.hpp"
#include "CameraMan.hpp"

#include "Edge.hpp"
#include "Cone.hpp"
#include "Cylinder.hpp"
#include "Ellipsoid.hpp"
#include "Plane.hpp"
#include "RectangularPrism.hpp"
#include "ObjMesh.hpp"

#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "SpotLight.hpp"

void RobotScene::init() {
    addRootObject(std::make_unique<DirectionalLight>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(-90.0f)),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        EngineUtil::ColorRGB(255, 255, 255),
        2.0f
    ));

    addRootObject(std::make_unique<PointLight>(
        EngineMath::Transform(
            EngineMath::Vector3(4.0f, 4.0f, -23.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        EngineUtil::ColorRGB(0, 255, 0),
        5.0f,
        11.0f
    ));

    addRootObject(std::make_unique<SpotLight>(
        EngineMath::Transform(
            EngineMath::Vector3(-4.0f, 6.5f, -21.5f),
            EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(-45.0f)),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        EngineUtil::ColorRGB(0, 0, 255),
        5.0f,
        14.0f,
        26.0f,
        42.0f
    ));

    EngineRender::Material testTexture = EngineRender::Material(
        EngineUtil::ColorRGB(255, 255, 255),
        0.5f,
        0.0f,
        "../Shaders/Basic.vert",
        "../Shaders/Textured.frag",
        "../Assets/Textures/test.png"
    );
    EngineRender::Material testTextureGray = EngineRender::Material(
        EngineUtil::ColorRGB(255, 255, 255),
        0.5f,
        0.0f,
        "../Shaders/Lit.vert",
        "../Shaders/Lit.frag",
        "../Assets/Textures/test2.png",
        {},
        {},
        {},
        50.0f,
        50.0f
    );
    EngineRender::Material testTextureLit = EngineRender::Material(
        EngineUtil::ColorRGB(255, 255, 255),
        0.5f,
        0.0f,
        "../Shaders/Lit.vert",
        "../Shaders/Lit.frag",
        "../Assets/Textures/test.png"
    );
    EngineRender::Material testNormal = EngineRender::Material(
        EngineUtil::ColorRGB(255, 255, 255),
        1.0f,
        0.0f,
        "../Shaders/Lit.vert",
        "../Shaders/Lit.frag",
        "../Assets/Textures/greenTile.png",
        "../Assets/Textures/greenTileNormal.png",
        "../Assets/Textures/greenTileRoughness.png"
    );
    EngineRender::Material testNormalRoughness = EngineRender::Material(
        EngineUtil::ColorRGB(255, 255, 255),
        1.0f,
        0.0f,
        "../Shaders/Lit.vert",
        "../Shaders/Lit.frag",
        "../Assets/Textures/ice.png",
        "../Assets/Textures/iceNormal.png",
        "../Assets/Textures/iceRoughness.png"
    );
    EngineRender::Material testNormalMetalness = EngineRender::Material(
        EngineUtil::ColorRGB(255, 255, 255),
        1.0f,
        0.25f,
        "../Shaders/Lit.vert",
        "../Shaders/Lit.frag",
        "../Assets/Textures/metal.png",
        "../Assets/Textures/metalNormal.png",
        "../Assets/Textures/metalRoughness.png",
        "../Assets/Textures/metalMetalness.png"
    );
    
    addRootObject(std::make_unique<Plane>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(100.0f, 1.0f, 100.0f)
        ),
        testTextureGray,
        4
    ));

    EngineRender::Material bounceMaterial = EngineRender::Material(
        EngineUtil::ColorRGB(100, 140, 220),
        0.5f,
        0.0f,
        "../Shaders/Basic.vert",
        "../Shaders/Bounce.frag"
    );

    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 2.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        testTextureLit
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 5.0f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        testNormal
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 7.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        testNormalRoughness
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 10.0f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        testNormalMetalness
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 12.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        bounceMaterial
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 15.0f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        testTexture
    );


    // I wanted to learn the non srand, rand, time way
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> randomPosX(-25.0f, 25.0f);
    std::uniform_real_distribution<float> randomPosY(10.0f, 25.0f);
    std::uniform_real_distribution<float> randomRotation(0.0f, 360.0f);
    std::uniform_int_distribution<int> randomColor(0, 255);

    for (int i = 0; i < 25; ++i) {
        addRootObject(std::make_unique<Robot>(
            EngineMath::Transform(
                EngineMath::Vector3(randomPosX(gen), 2.25f, randomPosY(gen)),
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
    std::cout << "ESC: Quit\n";
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

void RobotScene::buildDemo(EngineMath::Transform transform, EngineRender::Material material) {
    addRootObject(std::make_unique<Edge>(
        transform,
        material
    ));

    transform.position_.x_ += 3.0f;

    addRootObject(std::make_unique<Cone>(
        transform,
        material,
        16
    ));

    transform.position_.x_ += 3.0f;

    addRootObject(std::make_unique<Cylinder>(
        transform,
        material,
        16
    ));

    transform.position_.x_ += 3.0f;

    addRootObject(std::make_unique<Ellipsoid>(
        transform,
        material,
        16,
        16
    ));

    transform.position_.x_ += 3.0f;

    addRootObject(std::make_unique<RectangularPrism>(
        transform,
        material
    ));

    transform.position_.x_ += 3.0f;
    transform.scale_.y_ = 1.0f;
    transform.rotation_ = EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(45.0f));

    addRootObject(std::make_unique<Plane>(
        transform,
        material,
        4
    ));

    transform.position_.x_ += 3.0f;
    transform.scale_ = EngineMath::Vector3(0.5f, 0.5f, 0.5f);
    transform.rotation_ = EngineMath::Quaternion::identity();

    addRootObject(std::make_unique<ObjMesh>(
        transform,
        material,
        "../Assets/Objs/house.obj"
    ));
}