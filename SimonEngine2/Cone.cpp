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

    for (int slice = 0; slice < slices_; ++slice) {
        const auto& current = mesh_->vertices_[slice];
        const auto& next = mesh_->vertices_[slice + 1];

        glVertex3f(0.0f, 0.5f, 0.0f);
        glVertex3f(current.x_, current.y_, current.z_);
        glVertex3f(next.x_, next.y_, next.z_);
    }

    glEnd();

    glBegin(GL_TRIANGLE_FAN);

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