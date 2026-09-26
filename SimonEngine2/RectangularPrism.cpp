#include "RectangularPrism.hpp"

#include "Engine.hpp"

RectangularPrism::RectangularPrism(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) {
	localTransform_ = localTransform;

    activeColor_ = color;
    color_ = color;
    grayColor_ = EngineUtil::ColorRGB::toGrayScale(color);
}

void RectangularPrism::draw() {
    if (drawAsSolid_) {
        drawSolid();
    }
    else {
        drawEdges();
    }
}

void RectangularPrism::drawSolid() const {
    glDisable(GL_TEXTURE_2D);

    const float hx = 0.5f;
    const float hy = 0.5f;
    const float hz = 0.5f;

    glBegin(GL_QUADS);

    glColor3f(activeColor_.r_, activeColor_.g_, activeColor_.b_);

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

void RectangularPrism::drawEdges() const {
    glDisable(GL_TEXTURE_2D);

    const float hx = 0.5f;
    const float hy = 0.5f;
    const float hz = 0.5f;

    glColor3f(1.0f, 1.0f, 1.0f);;

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

void RectangularPrism::update() {
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