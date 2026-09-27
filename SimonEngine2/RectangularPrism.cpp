#include "RectangularPrism.hpp"

#include "Engine.hpp"

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