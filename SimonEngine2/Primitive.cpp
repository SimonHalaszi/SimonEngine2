#include "Primitive.hpp"

Primitive::Primitive(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) {
	localTransform_ = localTransform;

	activeColor_ = color;
	color_ = color;
	grayColor_ = EngineUtil::ColorRGB::toGrayScale(color);
}

void Primitive::draw() {
    if (drawAsSolid_) {
        drawSolid();
    }
    else {
        drawEdges();
    }
}

void Primitive::update() {
    if (InputManager::getInstance().isPressed('s')) {
        drawAsSolid_ = true;
    }
    if (InputManager::getInstance().isPressed('w')) {
        drawAsSolid_ = false;
    }
    if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_LEFT)) {
        drawColor_ = !drawColor_;
        if (drawColor_) {
            activeColor_ = color_;
        }
        else {
            activeColor_ = grayColor_;
        }
    }
}

void Primitive::setColor(EngineUtil::ColorRGB color) {
    color_ = color;
    grayColor_ = EngineUtil::ColorRGB::toGrayScale(color);

    if (drawColor_) {
        activeColor_ = color;
    }
    else {
        activeColor_ = grayColor_;
    }
}