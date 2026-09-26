#include "Cone.hpp"

#include "Engine.hpp"

namespace {
    constexpr int minSlices = 3;
}

Cone::Cone(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices) {
    localTransform_ = localTransform;
    radius_ = localTransform_.scale_.x_ / 2.0f;
    halfHeight_ = localTransform_.scale_.y_ / 2.0f;
    
    activeColor_ = color;
    color_ = color;
    grayColor_ = EngineUtil::ColorRGB::toGrayScale(color);

    slices_ = std::max(minSlices, slices);
    buildMesh();
}

void Cone::buildMesh() {
    vertices_.clear();

    for (int slice = 0; slice <= slices_; ++slice) {
        const float theta =
            2.0f * EngineMath::PI * static_cast<float>(slice) / slices_;

        const float x = radius_ * std::cos(theta);
        const float z = radius_ * std::sin(theta);

        vertices_.push_back({ x, -halfHeight_, z });
    }
}

void Cone::draw() {
    if (drawAsSolid_) {
        drawSolid();
    }
    else {
        drawEdges();
    }
}

void Cone::drawSolid() const {
    glDisable(GL_TEXTURE_2D);

    glColor3f(activeColor_.r_, activeColor_.g_, activeColor_.b_);

    glBegin(GL_TRIANGLES);

    for (int slice = 0; slice < slices_; ++slice) {
        const auto& current = vertices_[slice];
        const auto& next = vertices_[slice + 1];

        glVertex3f(0.0f, halfHeight_, 0.0f);
        glVertex3f(current.x_, current.y_, current.z_);
        glVertex3f(next.x_, next.y_, next.z_);
    }

    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    glVertex3f(0.0f, -halfHeight_, 0.0f);

    for (int slice = slices_; slice >= 0; --slice) {
        const auto& vertex = vertices_[slice];

        glVertex3f(vertex.x_, vertex.y_, vertex.z_);
    }

    glEnd();
}

void Cone::drawEdges() const {
    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);

    for (int slice = 0; slice < slices_; ++slice) {
        const auto& a = vertices_[slice];
        const auto& b = vertices_[slice + 1];

        glVertex3f(a.x_, a.y_, a.z_);
        glVertex3f(b.x_, b.y_, b.z_);
    }

    for (int slice = 0; slice < slices_; ++slice) {
        const auto& base = vertices_[slice];

        glVertex3f(base.x_, base.y_, base.z_);
        glVertex3f(0.0f, halfHeight_, 0.0f);
    }

    glEnd();
}


void Cone::update() {
    if (InputManager::getInstance().isPressed('s')) {
        drawAsSolid_ = true;
    }
    if (InputManager::getInstance().isPressed('w')) {
        drawAsSolid_ = false;
    }
    if (InputManager::getInstance().isMouseButtonPressed(MOUSEBUTTON_LEFT)) {
        drawColor_ = !drawColor_;
        if (drawColor_) {
            activeColor_ = color_;
        }
        else {
            activeColor_ = grayColor_;
        }
    }
}