#include "Cylinder.hpp"

#include "Engine.hpp"

namespace {
    constexpr int minSlices = 3;
}

Cylinder::Cylinder(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices) {
    localTransform_ = localTransform;
    radius_ = localTransform_.scale_.x_ / 2.0f;
    halfHeight_ = localTransform_.scale_.y_ / 2.0f;
    color_ = color;
    
    slices_ = std::max(minSlices, slices);
    buildMesh();
}

void Cylinder::buildMesh() {
    vertices_.clear();

    for (int ring = 0; ring < 2; ++ring) {
        const float y = (ring == 0) ? -halfHeight_ : halfHeight_;

        for (int slice = 0; slice <= slices_; ++slice) {
            const float theta =
                2.0f * EngineMath::PI * static_cast<float>(slice) / slices_;

            const float x = radius_ * std::cos(theta);
            const float z = radius_ * std::sin(theta);

            vertices_.push_back({ x, y, z });
        }
    }
}

const EngineUtil::Vertex& Cylinder::vertexAt(int ring, int slice) const {
    return vertices_[ring * (slices_ + 1) + slice];
}

void Cylinder::draw() {
    if (drawAsSolid_) {
        drawSolid();
    }
    else {
        drawEdges();
    }
}

void Cylinder::drawSolid() const {
    glDisable(GL_TEXTURE_2D);
    glColor3f(color_.r_, color_.g_, color_.b_);

    glBegin(GL_QUAD_STRIP);

    for (int slice = 0; slice <= slices_; ++slice) {
        const EngineUtil::Vertex& bottom = vertexAt(0, slice);
        const EngineUtil::Vertex& top = vertexAt(1, slice);

        glVertex3f(bottom.x_, bottom.y_, bottom.z_);
        glVertex3f(top.x_, top.y_, top.z_);
    }

    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0.0f, -halfHeight_, 0.0f);

    for (int slice = slices_; slice >= 0; --slice) {
        const EngineUtil::Vertex& v = vertexAt(0, slice);
        glVertex3f(v.x_, v.y_, v.z_);
    }

    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0.0f, halfHeight_, 0.0f);

    for (int slice = 0; slice <= slices_; ++slice) {
        const EngineUtil::Vertex& v = vertexAt(1, slice);
        glVertex3f(v.x_, v.y_, v.z_);
    }

    glEnd();
}

void Cylinder::drawEdges() const {
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


void Cylinder::update() {
    if (InputManager::getInstance().isPressed('s')) {
        drawAsSolid_ = true;
    }
    if (InputManager::getInstance().isPressed('w')) {
        drawAsSolid_ = false;
    }
}