#include "Primitive.hpp"

Primitive::Primitive(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) {
	localTransform_ = localTransform;
    color_ = color;
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
    if (InputManager::getInstance().isPressed('1')) {
        drawAsSolid_ = false;
    }
    if (InputManager::getInstance().isPressed('2')) {
        drawAsSolid_ = true;
    }
}