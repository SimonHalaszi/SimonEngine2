#include "RectangleObject.hpp"

#include "Engine.hpp"

RectangleObject::RectangleObject(const EngineMath::Transform& transform, const EngineUtil::ColorRGB& color) {
	localTransform_ = transform;
	color_ = color;
}

void RectangleObject::draw() {
    if (drawAsSolid_) {
        drawSolid();
    }
    else {
        drawEdges();
    }
}

void RectangleObject::drawSolid() const {
    glDisable(GL_TEXTURE_2D);

    const float hx = 0.5f;
    const float hy = 0.5f;
    const float hz = 0.5f;

    glBegin(GL_QUADS);

    glColor3f(color_.r_, color_.g_, color_.b_);

    glVertex3f(-hx, -hy, hz);
    glVertex3f(hx, -hy, hz);
    glVertex3f(hx, hy, hz);
    glVertex3f(-hx, hy, hz);

    glVertex3f(hx, -hy, -hz);
    glVertex3f(-hx, -hy, -hz);
    glVertex3f(-hx, hy, -hz);
    glVertex3f(hx, hy, -hz);

    glVertex3f(-hx, -hy, -hz);
    glVertex3f(-hx, -hy, hz);
    glVertex3f(-hx, hy, hz);
    glVertex3f(-hx, hy, -hz);

    glVertex3f(hx, -hy, hz);
    glVertex3f(hx, -hy, -hz);
    glVertex3f(hx, hy, -hz);
    glVertex3f(hx, hy, hz);

    glVertex3f(-hx, hy, hz);
    glVertex3f(hx, hy, hz);
    glVertex3f(hx, hy, -hz);
    glVertex3f(-hx, hy, -hz);

    glVertex3f(-hx, -hy, -hz);
    glVertex3f(hx, -hy, -hz);
    glVertex3f(hx, -hy, hz);
    glVertex3f(-hx, -hy, hz);

    glEnd();
}

void RectangleObject::drawEdges() const {
    glDisable(GL_TEXTURE_2D);

    const float hx = 0.5f;
    const float hy = 0.5f;
    const float hz = 0.5f;

    glColor3f(color_.r_, color_.g_, color_.b_);

    glBegin(GL_LINES);

    glVertex3f(-hx, -hy, hz); glVertex3f(hx, -hy, hz);
    glVertex3f(hx, -hy, hz); glVertex3f(hx, hy, hz);
    glVertex3f(hx, hy, hz); glVertex3f(-hx, hy, hz);
    glVertex3f(-hx, hy, hz); glVertex3f(-hx, -hy, hz);

    glVertex3f(-hx, -hy, -hz); glVertex3f(hx, -hy, -hz);
    glVertex3f(hx, -hy, -hz); glVertex3f(hx, hy, -hz);
    glVertex3f(hx, hy, -hz); glVertex3f(-hx, hy, -hz);
    glVertex3f(-hx, hy, -hz); glVertex3f(-hx, -hy, -hz);

    glVertex3f(-hx, -hy, hz); glVertex3f(-hx, -hy, -hz);
    glVertex3f(hx, -hy, hz); glVertex3f(hx, -hy, -hz);
    glVertex3f(hx, hy, hz); glVertex3f(hx, hy, -hz);
    glVertex3f(-hx, hy, hz); glVertex3f(-hx, hy, -hz);

    glEnd();
}

void RectangleObject::update() {
    if (InputManager::getInstance().isPressed('s')) {
        drawAsSolid_ = true;
    }
    if (InputManager::getInstance().isPressed('w')) {
        drawAsSolid_ = false;
    }
    if (InputManager::getInstance().isPressed('m')) {
        toggleDrawing();
    }
}