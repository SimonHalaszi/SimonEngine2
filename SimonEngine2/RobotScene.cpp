#include "RobotScene.hpp"

void RobotScene::init() {
    addRootObject(std::make_unique<Robot>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(1.0f, 1.0f, 1.0f)
        ),
        EngineUtil::ColorRGB(50, 185, 235)
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
    static RobotScene* menuScene = nullptr;
    menuScene = this;

    auto callback = [](int value) {
        if (menuScene) {
            menuScene->mainMenu(value);
        }
    };

    rotationXMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("30 Degrees", 1001);
    glutAddMenuEntry("45 Degrees", 1002);
    glutAddMenuEntry("90 Degrees", 1003);
    glutAddMenuEntry("180 Degrees", 1004);
    glutAddMenuEntry("270 Degrees", 1005);

    rotationYMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("30 Degrees", 2001);
    glutAddMenuEntry("45 Degrees", 2002);
    glutAddMenuEntry("90 Degrees", 2003);
    glutAddMenuEntry("180 Degrees", 2004);
    glutAddMenuEntry("270 Degrees", 2005);

    rotationZMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("30 Degrees", 3001);
    glutAddMenuEntry("45 Degrees", 3002);
    glutAddMenuEntry("90 Degrees", 3003);
    glutAddMenuEntry("180 Degrees", 3004);
    glutAddMenuEntry("270 Degrees", 3005);

    rotationMenu_ = glutCreateMenu(callback);
    glutAddSubMenu("X", rotationXMenu_);
    glutAddSubMenu("Y", rotationYMenu_);
    glutAddSubMenu("Z", rotationZMenu_);

    perspectiveMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("FOV: 30", 4002);
    glutAddMenuEntry("FOV: 75", 4003);

    renderMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("Orthographic", 4001);
    glutAddSubMenu("Perspective", perspectiveMenu_);

    colorMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("Red", 5001);
    glutAddMenuEntry("Orange", 5002);
    glutAddMenuEntry("Yellow", 5003);
    glutAddMenuEntry("Green", 5004);
    glutAddMenuEntry("Blue", 5005);
    glutAddMenuEntry("Purple", 5006);

    mainMenu_ = glutCreateMenu(callback);
    glutAddSubMenu("Rotate Object", rotationMenu_);
    glutAddSubMenu("Render Mode", renderMenu_);
    glutAddSubMenu("Color Body", colorMenu_);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    robot_ = getRobot();
}

void RobotScene::mainMenu(int value) {
    EngineMath::Transform wt;
    if (robot_) {
        wt = robot_->getWorldTransform();
    }
    
    switch (value) {
        case 1001:
            wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(30.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 1002:
            wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(45.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 1003:
            wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(90.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 1004:
            wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(180.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 1005:
            wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(270.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;

        case 2001:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(30.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 2002:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(45.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 2003:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(90.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 2004:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(180.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 2005:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(270.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;


        case 3001:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(30.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 3002:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(45.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 3003:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(90.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 3004:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(180.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;
        case 3005:
            wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(270.0f));
            if (robot_) {
                robot_->setTransform(wt);
            }
            break;

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

        case 5001:
            if (robot_) {
                robot_->setColor(EngineUtil::ColorRGB(235, 70, 50));
            }
            break;
        case 5002:
            if (robot_) {
                robot_->setColor(EngineUtil::ColorRGB(235, 145, 50));
            }
            break;
        case 5003:
            if (robot_) {
                robot_->setColor(EngineUtil::ColorRGB(235, 230, 50));
            }
            break;
        case 5004:
            if (robot_) {
                robot_->setColor(EngineUtil::ColorRGB(50, 235, 90));
            }
            break;
        case 5005:
            if (robot_) {
                robot_->setColor(EngineUtil::ColorRGB(50, 185, 235));
            }
            break;
        case 5006:
            if (robot_) {
                robot_->setColor(EngineUtil::ColorRGB(170, 50, 235));
            }
            break;
    }
}

#include "Engine.hpp"
#include "ObjScene.hpp"
#include "MagicCubeScene.hpp"

void RobotScene::update() {
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
        // Already here
    }
    if (InputManager::getInstance().isPressed('2')) {
        Engine::getInstance().changeScene(std::make_unique<ObjScene>());
    }
    if (InputManager::getInstance().isPressed('3')) {
        Engine::getInstance().changeScene(std::make_unique<MagicCubeScene>());
    }
}

void RobotScene::deInit() {
    glutDetachMenu(GLUT_RIGHT_BUTTON);
    
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

Robot* RobotScene::getRobot() {
    for (auto& obj : rootObjects_) {
        Robot* robot = dynamic_cast<Robot*>(obj.get());
        if (robot) {
            return robot;
        }
    }
    return nullptr;
}