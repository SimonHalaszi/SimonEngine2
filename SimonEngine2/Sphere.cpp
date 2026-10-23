#include "Sphere.hpp"

#include "MeshRegistry.hpp"

namespace {
    constexpr int minSlices = 3;
    constexpr int minStacks = 3; // (was called maxStacks, but it is very much a minimum)
}

Sphere::Sphere(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices, int stacks)
    : Primitive(localTransform, color)
{
    slices_ = std::max(minSlices, slices);
    stacks_ = std::max(minStacks, stacks);
    mesh_ = MeshRegistry::getInstance().getSphere(slices_, stacks_);
}

const EngineUtil::Vertex& Sphere::vertexAt(int stack, int slice) const {
    return mesh_->vertices_[stack * (slices_ + 1) + slice];
}

void Sphere::drawSolid() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(activeColor_.r_, activeColor_.g_, activeColor_.b_);

    for (int stack = 0; stack < stacks_; ++stack) {
        glBegin(GL_QUAD_STRIP);

        for (int slice = 0; slice <= slices_; ++slice) {
            const EngineUtil::Vertex& top = vertexAt(stack, slice);
            const EngineUtil::Vertex& bottom = vertexAt(stack + 1, slice);

            // On a sphere centered at the origin the normal is just the position, pointed
            // outward. The unit sphere has radius 0.5 here so scale by 2 to keep it unit length.
            glNormal3f(top.x_ * 2.0f, top.y_ * 2.0f, top.z_ * 2.0f);
            glVertex3f(top.x_, top.y_, top.z_);
            glNormal3f(bottom.x_ * 2.0f, bottom.y_ * 2.0f, bottom.z_ * 2.0f);
            glVertex3f(bottom.x_, bottom.y_, bottom.z_);
        }

        glEnd();
    }
}

void Sphere::drawEdges() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);;

    glBegin(GL_LINES);

    for (int slice = 0; slice < slices_; ++slice) {
        for (int stack = 0; stack < stacks_; ++stack) {
            const EngineUtil::Vertex& a = vertexAt(stack, slice);
            const EngineUtil::Vertex& b = vertexAt(stack + 1, slice);

            glVertex3f(a.x_, a.y_, a.z_);
            glVertex3f(b.x_, b.y_, b.z_);
        }
    }

    for (int stack = 1; stack < stacks_; ++stack) {
        for (int slice = 0; slice < slices_; ++slice) {
            const EngineUtil::Vertex& a = vertexAt(stack, slice);
            const EngineUtil::Vertex& b = vertexAt(stack, slice + 1);

            glVertex3f(a.x_, a.y_, a.z_);
            glVertex3f(b.x_, b.y_, b.z_);
        }
    }

    glEnd();
}