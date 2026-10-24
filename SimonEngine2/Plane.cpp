#include "Plane.hpp"

#include "Engine.hpp"

void Plane::drawSolid() const {
    glDisable(GL_TEXTURE_2D);

    glColor3f(color_.r_, color_.g_, color_.b_);

    glBegin(GL_QUADS);

    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);

    glEnd();
}

void Plane::drawEdges() const {
    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINE_LOOP);

    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);

    glEnd();
}