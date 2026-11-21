#include "RobotScene.hpp"

// Specific Objects
#include "CameraMan.hpp"
#include "ArcballCamera.hpp"

#include "PathedParent.hpp"
#include "Robot.hpp"
#include "AxisDisplay.hpp"

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
#include "CubeMap.hpp"
#include "Skybox.hpp"

namespace {
    std::vector<EngineMath::Vector3> buildCirclePath(const EngineMath::Vector3& pos, int pointCount, float radius) {
        std::vector<EngineMath::Vector3> positions;
        positions.reserve(pointCount);

        for (int i = 0; i < pointCount; ++i) {
            const float angle = 2.0f * EngineMath::PI * i / pointCount;
            positions.emplace_back(radius * std::cos(angle), pos.y_, radius * std::sin(angle));
        }

        return positions;
    }

    std::vector<EngineMath::Vector3> buildLinePath(const EngineMath::Vector3& pos1, const EngineMath::Vector3& pos2) {
        std::vector<EngineMath::Vector3> positions;
        positions.reserve(2);

        positions.emplace_back(pos1);
        positions.emplace_back(pos2);

        return positions;
    }

}

void RobotScene::init() {
    std::array<std::string, 6> facePaths = {
        "../Assets/Textures/CubeMaps/Overcast/px.png", // +X
        "../Assets/Textures/CubeMaps/Overcast/nx.png", // -X
        "../Assets/Textures/CubeMaps/Overcast/py.png", // +Y
        "../Assets/Textures/CubeMaps/Overcast/ny.png", // -Y
        "../Assets/Textures/CubeMaps/Overcast/pz.png", // +Z
        "../Assets/Textures/CubeMaps/Overcast/nz.png"  // -Z
    };

    GLuint cubeMapID = TextureRegistry::getInstance().loadCubeMap(facePaths);

    addRootObject(std::make_unique<CubeMap>(EngineMath::Transform(), cubeMapID));
    addRootObject(std::make_unique<Skybox>(cubeMapID));

    addRootObject(std::make_unique<DirectionalLight>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::fromAxisAngle(EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(-60.0f)),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        EngineUtil::ColorRGB(200, 200, 200),
        5.0f
    ));

    MaterialRegistry::getInstance().getOrCreate("green", EngineRender::Material(EngineUtil::ColorRGB(0, 255, 0)));
    addRootObject(std::make_unique<PathedParent>(
        std::make_unique<PointLight>(
            EngineMath::Transform(
                EngineMath::Vector3(0.0f, 0.0f, 0.0f),
                EngineMath::Quaternion::identity(),
                EngineMath::Vector3(1.0f, 1.0f, 1.0f)
            ),
            EngineUtil::ColorRGB(0, 255, 0),
            5.0f,
            11.0f
        ),
        buildLinePath(EngineMath::Vector3(-12.0f, 2.5f, -23.5f), EngineMath::Vector3(12.0f, 7.5f, -23.5f)),
        5.0f,
        MaterialRegistry::getInstance().get("green")
    ));

    MaterialRegistry::getInstance().getOrCreate("blue", EngineRender::Material(EngineUtil::ColorRGB(0, 0, 255)));
    addRootObject(std::make_unique<PathedParent>(
        std::make_unique<SpotLight>(
            EngineMath::Transform(
                EngineMath::Vector3(0.0f, 0.0f, 0.0f),
                EngineMath::Quaternion::identity(),
                EngineMath::Vector3(1.0f, 1.0f, 1.0f)
            ),
            EngineUtil::ColorRGB(0, 0, 255),
            5.0f,
            14.0f,
            26.0f,
            42.0f
        ),
        buildLinePath(EngineMath::Vector3(12.0f, 2.5f, -23.5f), EngineMath::Vector3(-12.0f, 7.5f, -23.5f)),
        5.0f,
        MaterialRegistry::getInstance().get("blue")
    ));

    MaterialRegistry::getInstance().getOrCreate(
        "testTextureGray", 
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            1.0f,
            0.0f,
            "../Shaders/Lit.vert",
            "../Shaders/Lit.frag",
            "../Assets/Textures/test2.png",
            {},
            {},
            {},
            50.0f,
            50.0f
        )
    );
    addRootObject(std::make_unique<Plane>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(100.0f, 1.0f, 100.0f)
        ),
        MaterialRegistry::getInstance().get("testTextureGray"),
        4
    ));

    MaterialRegistry::getInstance().getOrCreate(
        "testTextureLit",
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            1.0f,
            0.0f,
            "../Shaders/Lit.vert",
            "../Shaders/Lit.frag",
            "../Assets/Textures/test.png"
        )
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 2.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        MaterialRegistry::getInstance().get("testTextureLit")
    );


    MaterialRegistry::getInstance().getOrCreate(
        "testNormal",
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            1.0f,
            1.0f,
            "../Shaders/Lit.vert",
            "../Shaders/Lit.frag",
            "../Assets/Textures/mossyBrick.png",
            "../Assets/Textures/mossyBrickNormal.png",
            "../Assets/Textures/mossyBrickRoughness.png"
        )
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 5.0f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        MaterialRegistry::getInstance().get("testNormal")
    );

    MaterialRegistry::getInstance().getOrCreate(
        "testNormalRoughness",
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            1.0f,
            0.0f,
            "../Shaders/Lit.vert",
            "../Shaders/Lit.frag",
            "../Assets/Textures/ice.png",
            "../Assets/Textures/iceNormal.png",
            "../Assets/Textures/iceRoughness.png"
        )
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 7.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        MaterialRegistry::getInstance().get("testNormalRoughness")
    );

    MaterialRegistry::getInstance().getOrCreate(
        "testNormalMetalness",
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            1.0f,
            1.0f,
            "../Shaders/Lit.vert",
            "../Shaders/Lit.frag",
            "../Assets/Textures/metal.png",
            "../Assets/Textures/metalNormal.png",
            "../Assets/Textures/metalRoughness.png",
            "../Assets/Textures/metalMetalness.png"
        )
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 10.0f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        MaterialRegistry::getInstance().get("testNormalMetalness")
    );

    MaterialRegistry::getInstance().getOrCreate(
        "testShader",
        EngineRender::Material(
            EngineUtil::ColorRGB(100, 140, 220),
            0.5f,
            0.0f,
            "../Shaders/Basic.vert",
            "../Shaders/Bounce.frag"
        )
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 12.5f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        MaterialRegistry::getInstance().get("testShader")
    );

    MaterialRegistry::getInstance().getOrCreate(
        "testTextureUnlit",
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            0.5f,
            0.0f,
            "../Shaders/Basic.vert",
            "../Shaders/Textured.frag",
            "../Assets/Textures/test.png"
        )
    );
    buildDemo(
        EngineMath::Transform(
            EngineMath::Vector3(-9.0f, 15.0f, -25.5f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(2.0f, 2.0f, 2.0f)
        ),
        MaterialRegistry::getInstance().get("testTextureUnlit")
    );

    MaterialRegistry::getInstance().getOrCreate(
        "skin",
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            1.0f,
            0.0f,
            "../Shaders/Lit.vert",
            "../Shaders/Lit.frag",
            "../Assets/Textures/skin.png",
            "../Assets/Textures/skinNormal.png",
            "../Assets/Textures/skinRoughness.png"
        )
    );
    MaterialRegistry::getInstance().getOrCreate(
        "shirt",
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            1.0f,
            0.0f,
            "../Shaders/Lit.vert",
            "../Shaders/Lit.frag",
            "../Assets/Textures/fabric.png",
            "../Assets/Textures/fabricNormal.png",
            "../Assets/Textures/fabricRoughness.png"
        )
    );
    MaterialRegistry::getInstance().getOrCreate(
        "pants",
        EngineRender::Material(
            EngineUtil::ColorRGB(255, 255, 255),
            1.0f,
            0.0f,
            "../Shaders/Lit.vert",
            "../Shaders/Lit.frag",
            "../Assets/Textures/pants.png",
            "../Assets/Textures/pantsNormal.png",
            "../Assets/Textures/pantsRoughness.png"
        )
    );
    const EngineRender::Material* red = MaterialRegistry::getInstance().getOrCreate("red", EngineRender::Material(EngineUtil::ColorRGB(255, 0, 0)));
    std::unique_ptr<PathedParent> robotPather = std::make_unique<PathedParent>(
        std::make_unique<Robot>(
            EngineMath::Transform(
                EngineMath::Vector3(0.0f, 0.0f, 0.0f),
                EngineMath::Quaternion::identity(),
                EngineMath::Vector3(1.0f, 1.0f, 1.0f)
            ),
            MaterialRegistry::getInstance().get("skin"),
            MaterialRegistry::getInstance().get("shirt"),
            MaterialRegistry::getInstance().get("pants")
        ),
        std::vector<EngineMath::Vector3>(),
        5.0f,
        MaterialRegistry::getInstance().get("red")
    );
    robotPather_ = robotPather.get();
    addRootObject(std::move(robotPather));
    changePath();

    addRootObject(std::make_unique<AxisDisplay>());

    //addRootObject(std::make_unique<CameraMan>(
    //    EngineMath::Transform(
    //        EngineMath::Vector3(0.0f, 3.5f, 5.0f),
    //        EngineMath::Quaternion::identity(),
    //        EngineMath::Vector3(1.0f, 1.0f, 1.0f)
    //    )
    //));

    addRootObject(std::make_unique<ArcballCamera>(EngineMath::Vector3(0.0f, 0.0f, 0.0f), 10.0f));

    std::cout << "\n"
        << "Asset Credits:\n"
        << "All material assets come from ambientCG\n";
    
    std::cout << "\n"
        << "Controls:\n"
        << "1: Show primitive mesh as wires\n"
        << "2: Show primitive mesh as solids\n"
        << "3: Toggle axis display\n"
        << "4/F4: Increase/Decrease angle of elbows\n"
        << "5/F5: Increase/Decrease angle of shoulders\n"
        << "6/F6: Increase/Decrease angle of hips\n"
        << "7/F7: Increase/Decrease angle of knees\n"
        << "Arrow Keys: Tank controls\n"
        << "Space/LCtrl: Fly up/down\n"
        << "0: Print FPS\n"
        << "ESC: Quit\n";

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
    if (InputManager::getInstance().isPressed('p')) {
        changePath();
    }
}

void RobotScene::draw() const {

}

void RobotScene::deInit() {
    robotPather_ = nullptr;
}

void RobotScene::buildDemo(EngineMath::Transform transform, const EngineRender::Material* material) {
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

void RobotScene::changePath() {
    PathedParent* obj = dynamic_cast<PathedParent*>(robotPather_);

    constexpr int totalOptions = 2;
    
    if (obj) {
        if (pathOption_ == 0) {
            obj->setPositionsVector(buildCirclePath(EngineMath::Vector3(0.0f, 2.5f, 0.0f), 16, 5.0f));
        }
        if (pathOption_ == 1) {
            obj->setPositionsVector(buildLinePath(EngineMath::Vector3(0.0f, 2.5f, 0.0f), EngineMath::Vector3(5.0f, 2.5f, 15.0f)));
        }
    }

    pathOption_ = (pathOption_ + 1) % totalOptions;
}