#include "RectangleObject.hpp"

#include "Engine.hpp"

RectangleObject::RectangleObject(const EngineMath::Transform& transform, const EngineUtil::ColorRGB& color) {
	localTransform_ = transform;
	color_ = color;

    int updateSpeed = Engine::getInstance().getCurrentScene()->getUpdateSpeed();
    rotationSpeed_ = EngineMath::degreesToRadians(90.0f / updateSpeed);
}

void RectangleObject::onStart() {

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
    rotationRadians_ += rotationSpeed_;

    setRotation(
        EngineMath::Quaternion::fromXYZ(
            rotationRadians_ * 0.75f,
            rotationRadians_,
            rotationRadians_ * 0.25f
        )
    );
}