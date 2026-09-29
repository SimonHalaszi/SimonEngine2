#include "BasicScene.hpp"

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
        ),
        EngineUtil::ColorRGB(125, 125, 125)
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
    camera_.setPosition(EngineMath::Vector3(0.0f, 0.0f, -5.0f));

    // Making menu
    static BasicScene* menuScene = nullptr;
    menuScene = this;

    auto callback = [](int value) {
        if (menuScene) {
            menuScene->mainMenu(value);
        }
    };

    rotationXMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("45 Degrees", 1);
    glutAddMenuEntry("90 Degrees", 2);

    rotationYMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("45 Degrees", 3);
    glutAddMenuEntry("90 Degrees", 4);

    rotationZMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("45 Degrees", 5);
    glutAddMenuEntry("90 Degrees", 6);

    rotationMenu_ = glutCreateMenu(callback);
    glutAddSubMenu("X", rotationXMenu_);
    glutAddSubMenu("Y", rotationYMenu_);
    glutAddSubMenu("Z", rotationZMenu_);

    perspectiveMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("FOV: 60", 7);
    glutAddMenuEntry("FOV: 120", 8);

    renderMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("Orthographic", 9);
    glutAddSubMenu("Perspective", perspectiveMenu_);

    colorMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("Red", 10);
    glutAddMenuEntry("Green", 11);
    glutAddMenuEntry("Blue", 12);

    mainMenu_ = glutCreateMenu(callback);
    glutAddSubMenu("Rotate Camera", rotationMenu_);
    glutAddSubMenu("Render Mode", renderMenu_);
    glutAddSubMenu("Color Body", colorMenu_);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void BasicScene::mainMenu(int value) {
    Robot* robot = getRobot();
    EngineMath::Transform wt = robot->getWorldTransform();
    
    switch (value) {
        case 1: 
            wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(45.0f));
            robot->setTransform(wt);
            break;
        case 2:
            wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(90.0f));
            robot->setTransform(wt);
            break;
        case 3:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(45.0f));
            robot->setTransform(wt);
            break;
        case 4:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(90.0f));
            robot->setTransform(wt);
            break;
        case 5:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(45.0f));
            robot->setTransform(wt);
            break;
        case 6:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(90.0f));
            robot->setTransform(wt);
            break;
        case 7:
            camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
            camera_.setFOV(60.0);
            break;
        case 8:
            camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
            camera_.setFOV(120.0);
            break;
        case 9:
            camera_.setProjectionMode(Camera::ProjectionMode::Ortho);
            break;
        case 10:
            robot->changeColor(EngineUtil::ColorRGB(255, 120, 120));
            break;
        case 11:
            robot->changeColor(EngineUtil::ColorRGB(120, 255, 120));
            break;
        case 12:
            robot->changeColor(EngineUtil::ColorRGB(120, 120, 255));
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

void BasicScene::deInit() {
    if (rotationXMenu_ != 0) {
        glutDestroyMenu(rotationXMenu_);
    }
    if (rotationYMenu_ != 0) {
        glutDestroyMenu(rotationYMenu_);
    }
    if (rotationZMenu_ != 0) {
        glutDestroyMenu(rotationZMenu_);
    }
    if (rotationMenu_ != 0) {
        glutDestroyMenu(rotationMenu_);
    }

    if (perspectiveMenu_ != 0) {
        glutDestroyMenu(perspectiveMenu_);
    }
    if (renderMenu_ != 0) {
        glutDestroyMenu(renderMenu_);
    }

    if (colorMenu_ != 0) {
        glutDestroyMenu(colorMenu_);
    }

    if (mainMenu_ != 0) {
        glutDestroyMenu(mainMenu_);
    }
}

Robot* BasicScene::getRobot() {
    for (auto& obj : rootObjects_) {
        Robot* robot = dynamic_cast<Robot*>(obj.get());
        if (robot) {
            return robot;
        }
    }
    return nullptr;
}