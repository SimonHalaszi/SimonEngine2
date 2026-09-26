#include "BasicScene.hpp"

#include "MagicCube.hpp"
#include "Robot.hpp"
#include "AxisDisplay.hpp"

void BasicScene::init() {
    //addRootObject(std::make_unique<MagicCube>(
    //    EngineMath::Transform(
    //        EngineMath::Vector3(0.0f, 0.0f, 0.0f),
    //        EngineMath::Quaternion::identity(),
    //        EngineMath::Vector3(1.0f, 1.0f, 1.0f)
    //    )
    //));
    addRootObject(std::make_unique<Robot>(
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
    int rotationMenu = glutCreateMenu(BasicScene::mainMenu);
    glutAddMenuEntry("30 Degrees", 1);
    glutAddMenuEntry("60 Degrees", 2);
    glutAddMenuEntry("90 Degrees", 3);
    glutAddMenuEntry("120 Degrees", 4);
    glutAddMenuEntry("180 Degrees", 5);

    int renderMenu = glutCreateMenu(BasicScene::mainMenu);
    glutAddMenuEntry("Orthographic", 6);
    glutAddMenuEntry("Perspective", 7);

    int colorMenu = glutCreateMenu(BasicScene::mainMenu);
    glutAddMenuEntry("Red", 8);
    glutAddMenuEntry("Green", 9);
    glutAddMenuEntry("Blue", 10);

    glutCreateMenu(BasicScene::mainMenu);
    glutAddSubMenu("Rotate Camera", rotationMenu);
    glutAddSubMenu("Render Mode", renderMenu);
    glutAddSubMenu("Color Body", colorMenu);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void BasicScene::mainMenu(int value) {
    switch (value) {
        case 1: 
            std::cout << "30 Degree Rotation to Camera\n"; 
            break;
        case 2:
            std::cout << "60 Degree Rotation to Camera\n";
            break;
        case 3:
            std::cout << "90 Degree Rotation to Camera\n";
            break;
        case 4:
            std::cout << "120 Degree Rotation to Camera\n";
            break;
        case 5:
            std::cout << "180 Degree Rotation to Camera\n";
            break;
        case 6:
            std::cout << "Orthographic view\n";
            break;
        case 7:
            draw();
            break;
        case 8:
            std::cout << "Red body\n";
            break;
        case 9:
            std::cout << "Green body\n";
            break;
        case 10:
            std::cout << "Blue body\n";
            break;
    }
}

void BasicScene::draw() const {

}

void BasicScene::update() {
    if (InputManager::getInstance().isPressed('p')) {
        camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
    }

    if (InputManager::getInstance().isPressed('o')) {
        camera_.setProjectionMode(Camera::ProjectionMode::Ortho);
    }

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

    if (InputManager::getInstance().isPressed('k')) {
        camera_.setFOV(120.0);
    }

    if (InputManager::getInstance().isPressed('l')) {
        camera_.setFOV(60.0);
    }
}