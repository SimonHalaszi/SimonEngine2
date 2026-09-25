#include "Line.hpp"

#include "Engine.hpp"

Line::Line(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color) {
    localTransform_ = localTransform;
    color_ = color;
}

void Line::draw() {
    glDisable(GL_TEXTURE_2D);

    glColor3f(color_.r_, color_.g_, color_.b_);

    glBegin(GL_LINES);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);

    glEnd();
}