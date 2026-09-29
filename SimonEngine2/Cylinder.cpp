#include "Cylinder.hpp"

#include "MeshRegistry.hpp"

namespace {
    constexpr int minSlices = 3;
}

Cylinder::Cylinder(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices)
    : Primitive(localTransform, color)
{
    slices_ = std::max(minSlices, slices);
    mesh_ = MeshRegistry::getInstance().getCylinder(slices_);
}

const EngineUtil::Vertex& Cylinder::vertexAt(int ring, int slice) const {
    return mesh_->vertices_[ring * (slices_ + 1) + slice];
}

void Cylinder::drawSolid() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(activeColor_.r_, activeColor_.g_, activeColor_.b_);

    glBegin(GL_QUAD_STRIP);

    for (int slice = 0; slice <= slices_; ++slice) {
        const EngineUtil::Vertex& bottom = vertexAt(0, slice);
        const EngineUtil::Vertex& top = vertexAt(1, slice);

        glVertex3f(bottom.x_, bottom.y_, bottom.z_);
        glVertex3f(top.x_, top.y_, top.z_);
    }

    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0.0f, -0.5f, 0.0f);

    for (int slice = slices_; slice >= 0; --slice) {
        const EngineUtil::Vertex& v = vertexAt(0, slice);
        glVertex3f(v.x_, v.y_, v.z_);
    }

    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0.0f, 0.5f, 0.0f);

    for (int slice = 0; slice <= slices_; ++slice) {
        const EngineUtil::Vertex& v = vertexAt(1, slice);
        glVertex3f(v.x_, v.y_, v.z_);
    }

    glEnd();
}

void Cylinder::drawEdges() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);

    for (int slice = 0; slice < slices_; ++slice) {
        const EngineUtil::Vertex& a = vertexAt(0, slice);
        const EngineUtil::Vertex& b = vertexAt(0, slice + 1);

        glVertex3f(a.x_, a.y_, a.z_);
        glVertex3f(b.x_, b.y_, b.z_);
    }

    for (int slice = 0; slice < slices_; ++slice) {
        const EngineUtil::Vertex& a = vertexAt(1, slice);
        const EngineUtil::Vertex& b = vertexAt(1, slice + 1);

        glVertex3f(a.x_, a.y_, a.z_);
        glVertex3f(b.x_, b.y_, b.z_);
    }

    for (int slice = 0; slice < slices_; ++slice) {
        const EngineUtil::Vertex& a = vertexAt(0, slice);
        const EngineUtil::Vertex& b = vertexAt(1, slice);

        glVertex3f(a.x_, a.y_, a.z_);
        glVertex3f(b.x_, b.y_, b.z_);
    }

    glEnd();
}