#include "MagicCubeScene.hpp"

void MagicCubeScene::init() {
    addRootObject(std::make_unique<MagicCube>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        )
    ));
    addRootObject(std::make_unique<AxisDisplay>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        )
    ));

    std::unique_ptr<Object>& first = rootObjects_.front();

    camera_.setTarget(first->getWorldTransform().position_);
    camera_.setPosition(EngineMath::Vector3(5.0f, 5.0f, 5.0f));

    // Making menu
    static MagicCubeScene* menuScene = nullptr;
    menuScene = this;

    auto callback = [](int value) {
        if (menuScene) {
            menuScene->mainMenu(value);
        }
    };

    perspectiveMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("FOV: 30", 4002);
    glutAddMenuEntry("FOV: 75", 4003);

    renderMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("Orthographic", 4001);
    glutAddSubMenu("Perspective", perspectiveMenu_);


    mainMenu_ = glutCreateMenu(callback);
    glutAddSubMenu("Render Mode", renderMenu_);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    magicCube_ = getMagicCube();
}

void MagicCubeScene::mainMenu(int value) {
    switch (value) {
        case 4001:
            camera_.setProjectionMode(Camera::ProjectionMode::Ortho);
            break;
        case 4002:
            camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
            camera_.setFOV(30.0);
            break;
        case 4003:
            camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
            camera_.setFOV(75.0);
            break;
    }
}

#include "Engine.hpp"
#include "ObjScene.hpp"
#include "RobotScene.hpp"

void MagicCubeScene::update() {
    const bool isPerspective = (camera_.getProjectionMode() == Camera::ProjectionMode::Perspective);
    const double zoomAmount = isPerspective ? 2.0 : 0.05;

    if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_SCROLLUP)) {
        camera_.zoomIn(zoomAmount);
    }
    if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_SCROLLDOWN)) {
        camera_.zoomOut(zoomAmount);
    }

    if (InputManager::getInstance().isPressed('c')) {
        isDrawing_ = !isDrawing_;
    }

    if (InputManager::getInstance().isPressed('1')) {
        Engine::getInstance().changeScene(std::make_unique<RobotScene>());
    }
    if (InputManager::getInstance().isPressed('2')) {
        Engine::getInstance().changeScene(std::make_unique<ObjScene>());
    }
    if (InputManager::getInstance().isPressed('3')) {
        // Already here
    }
}

void MagicCubeScene::deInit() {
    glutDetachMenu(GLUT_RIGHT_BUTTON);

    if (perspectiveMenu_ != 0) {
        glutDestroyMenu(perspectiveMenu_);
    }
    if (renderMenu_ != 0) {
        glutDestroyMenu(renderMenu_);
    }

    if (mainMenu_ != 0) {
        glutDestroyMenu(mainMenu_);
    }
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