#include "Line.hpp"

#include "Engine.hpp"

Line::Line(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) {
    localTransform_ = localTransform;

    activeColor_ = color;
    color_ = color;
    grayColor_ = EngineUtil::ColorRGB::toGrayScale(color);
}

void Line::draw() {
    if (drawAsSolid_) {
        drawSolid();
    }
    else {
        drawEdges();
    }
}

void Line::drawSolid() const {
    glDisable(GL_TEXTURE_2D);

    glColor3f(activeColor_.r_, activeColor_.g_, activeColor_.b_);

    glBegin(GL_LINES);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);

    glEnd();
}

void Line::drawEdges() const {
    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);

    glEnd();
}

void Line::update() {
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