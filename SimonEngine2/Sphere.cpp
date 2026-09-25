#include "Sphere.hpp"

#include "Engine.hpp"

namespace {
    constexpr int minSlices = 3;
    constexpr int maxStacks = 3;
}

Sphere::Sphere(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices, int stacks) {
    localTransform_ = localTransform;
    radius_ = localTransform_.scale_.x_ / 2.0f;
    color_ = color;

    slices_ = std::max(minSlices, slices);
    stacks_ = std::max(maxStacks, stacks);
    buildMesh();
}

void Sphere::buildMesh() {
    vertices_.clear();

    for (int stack = 0; stack <= stacks_; ++stack) {
        const float phi = EngineMath::PI * static_cast<float>(stack) / stacks_;

        for (int slice = 0; slice <= slices_; ++slice) {
            const float theta = 2.0f * EngineMath::PI * static_cast<float>(slice) / slices_;

            const float x = std::sin(phi) * std::cos(theta);
            const float y = std::cos(phi);
            const float z = std::sin(phi) * std::sin(theta);

            vertices_.push_back({radius_ * x, radius_ * y, radius_ * z});
        }
    }
}

const EngineUtil::Vertex& Sphere::vertexAt(int stack, int slice) const {
    return vertices_[stack * (slices_ + 1) + slice];
}

void Sphere::draw() {
    if (drawAsSolid_) {
        drawSolid();
    }
    else {
        drawEdges();
    }
}

void Sphere::drawSolid() const {
    glDisable(GL_TEXTURE_2D);
    glColor3f(color_.r_, color_.g_, color_.b_);

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
    glDisable(GL_TEXTURE_2D);
    glColor3f(color_.r_, color_.g_, color_.b_);

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

void Sphere::update() {
    if (InputManager::getInstance().isPressed('s')) {
        drawAsSolid_ = true;
    }
    if (InputManager::getInstance().isPressed('w')) {
        drawAsSolid_ = false;
    }
}