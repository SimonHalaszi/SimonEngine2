#include "Line.hpp"

#include "Engine.hpp"

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