#include "Cone.hpp"

#include "MeshRegistry.hpp"

namespace {
    constexpr int minSlices = 3;
}

Cone::Cone(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices)
    : Primitive(localTransform, color)
{
    slices_ = std::max(minSlices, slices);
    mesh_ = MeshRegistry::getInstance().getCone(slices_);
}

void Cone::drawSolid() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(activeColor_.r_, activeColor_.g_, activeColor_.b_);

    glBegin(GL_TRIANGLES);

    // Side normals for a cone with radius 0.5 and height 1: the surface leans inward by
    // radius/height, so the outward normal at angle theta is (cos, 0.5, sin) before
    // normalizing. Base ring vertices are (0.5cos, -0.5, 0.5sin), so 2*x == cos.
    // The apex gets the normal halfway between its two base corners, which makes
    // the shading smooth instead of looking like a disco ball.
    const auto sideNormal = [](float x, float z) {
        return EngineMath::Vector3(2.0f * x, 0.5f, 2.0f * z).normalized();
    };

    for (int slice = 0; slice < slices_; ++slice) {
        const auto& current = mesh_->vertices_[slice];
        const auto& next = mesh_->vertices_[slice + 1];

        const EngineMath::Vector3 nCurrent = sideNormal(current.x_, current.z_);
        const EngineMath::Vector3 nNext = sideNormal(next.x_, next.z_);
        const EngineMath::Vector3 nApex = (nCurrent + nNext).normalized();

        glNormal3f(nApex.x_, nApex.y_, nApex.z_);
        glVertex3f(0.0f, 0.5f, 0.0f);
        glNormal3f(nCurrent.x_, nCurrent.y_, nCurrent.z_);
        glVertex3f(current.x_, current.y_, current.z_);
        glNormal3f(nNext.x_, nNext.y_, nNext.z_);
        glVertex3f(next.x_, next.y_, next.z_);
    }

    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    // The base is a flat disk facing straight down
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, -0.5f, 0.0f);

    for (int slice = slices_; slice >= 0; --slice) {
        const auto& vertex = mesh_->vertices_[slice];

        glVertex3f(vertex.x_, vertex.y_, vertex.z_);
    }

    glEnd();
}

void Cone::drawEdges() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);

    for (int slice = 0; slice < slices_; ++slice) {
        const auto& a = mesh_->vertices_[slice];
        const auto& b = mesh_->vertices_[slice + 1];

        glVertex3f(a.x_, a.y_, a.z_);
        glVertex3f(b.x_, b.y_, b.z_);
    }

    for (int slice = 0; slice < slices_; ++slice) {
        const auto& base = mesh_->vertices_[slice];

        glVertex3f(base.x_, base.y_, base.z_);
        glVertex3f(0.0f, 0.5f, 0.0f);
    }

    glEnd();
}