#include "Primitive.hpp"

Primitive::Primitive(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) {
	localTransform_ = localTransform;

	activeColor_ = color;
	color_ = color;
	grayColor_ = EngineUtil::ColorRGB::toGrayScale(color);
}

void Primitive::draw() {
    // Wireframe edges and Lines are pure emissive neon: they skip the lighting rig.
    // glPushAttrib(GL_ENABLE_BIT) snapshots every glEnable/glDisable flag, so we can turn
    // lighting off for just this primitive and get the exact previous state back after.
    const bool unlit = !drawAsSolid_ || !receivesLighting();

    if (unlit) {
        glPushAttrib(GL_ENABLE_BIT);
        glDisable(GL_LIGHTING);
    }

    if (drawAsSolid_) {
        drawSolid();
    }
    else {
        drawEdges();
    }

    if (unlit) {
        glPopAttrib();
    }
}

void Primitive::update() {
    if (InputManager::getInstance().isPressed('1')) {
        drawAsSolid_ = false;
    }
    if (InputManager::getInstance().isPressed('2')) {
        drawAsSolid_ = true;
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