#include "Sphere.hpp"

#include "MeshRegistry.hpp"

namespace {
    constexpr int minSlices = 3;
    constexpr int maxStacks = 3;
}

Sphere::Sphere(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices, int stacks)
    : Primitive(localTransform, color)
{
    slices_ = std::max(minSlices, slices);
    stacks_ = std::max(maxStacks, stacks);
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

            glVertex3f(top.x_, top.y_, top.z_);
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