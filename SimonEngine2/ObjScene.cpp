#include "ObjScene.hpp"

void ObjScene::init() {
    addRootObject(std::make_unique<ObjMesh>(
        EngineMath::Transform(
            EngineMath::Vector3(0.0f, 0.0f, 0.0f),
            EngineMath::Quaternion::identity(),
            EngineMath::Vector3(0.25f, 0.25f, 0.25f)
        ),
        EngineUtil::ColorRGB(125, 125, 125),
        "../Assets/teapot.obj"
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
    static ObjScene* menuScene = nullptr;
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
    glutAddMenuEntry("FOV: 30", 7);
    glutAddMenuEntry("FOV: 75", 8);

    renderMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("Orthographic", 9);
    glutAddSubMenu("Perspective", perspectiveMenu_);

    colorMenu_ = glutCreateMenu(callback);
    glutAddMenuEntry("Red", 10);
    glutAddMenuEntry("Green", 11);
    glutAddMenuEntry("Blue", 12);

    mainMenu_ = glutCreateMenu(callback);
    glutAddSubMenu("Rotate Object", rotationMenu_);
    glutAddSubMenu("Render Mode", renderMenu_);
    glutAddSubMenu("Color Body", colorMenu_);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void ObjScene::mainMenu(int value) {
    Object* object = getObjMesh();
    EngineMath::Transform wt;
    if (object) {
        wt = object->getWorldTransform();
    }

    Primitive* prim = dynamic_cast<Primitive*>(object);

    switch (value) {
    case 1:
        wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(45.0f));
        if (object) {
            object->setTransform(wt);
        }
        break;
    case 2:
        wt.rotateAround(wt.position_, EngineMath::Vector3(1.0f, 0.0f, 0.0f), EngineMath::degreesToRadians(90.0f));
        if (object) {
            object->setTransform(wt);
        }
        break;
    case 3:
        wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(45.0f));
        if (object) {
            object->setTransform(wt);
        }
        break;
    case 4:
        wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 1.0f, 0.0f), EngineMath::degreesToRadians(90.0f));
        if (object) {
            object->setTransform(wt);
        }
        break;
    case 5:
        wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(45.0f));
        if (object) {
            object->setTransform(wt);
        }
        break;
    case 6:
        wt.rotateAround(wt.position_, EngineMath::Vector3(0.0f, 0.0f, 1.0f), EngineMath::degreesToRadians(90.0f));
        if (object) {
            object->setTransform(wt);
        }
        break;
    case 7:
        camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
        camera_.setFOV(30.0);
        break;
    case 8:
        camera_.setProjectionMode(Camera::ProjectionMode::Perspective);
        camera_.setFOV(75.0);
        break;
    case 9:
        camera_.setProjectionMode(Camera::ProjectionMode::Ortho);
        break;
    case 10:
        if (prim) {
            prim->setColor(EngineUtil::ColorRGB(255, 120, 120));
        }
        break;
    case 11:
        if (prim) {
            prim->setColor(EngineUtil::ColorRGB(120, 255, 120));
        }
        break;
    case 12:
        if (prim) {
            prim->setColor(EngineUtil::ColorRGB(120, 120, 255));
        }
        break;
    }
}

void ObjScene::draw() const {

}

#include "Engine.hpp"
#include "BasicScene.hpp"

void ObjScene::update() {
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

    if (InputManager::getInstance().isPressed('l')) {
        Engine::getInstance().changeScene(std::make_unique<BasicScene>());
    }
}

void ObjScene::deInit() {
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

ObjMesh* ObjScene::getObjMesh() {
    for (auto& obj : rootObjects_) {
        ObjMesh* objectMesh = dynamic_cast<ObjMesh*>(obj.get());
        if (objectMesh) {
            return objectMesh;
        }
    }
    return nullptr;
}