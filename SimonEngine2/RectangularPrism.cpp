#include "RectangularPrism.hpp"

#include "MeshRegistry.hpp"

RectangularPrism::RectangularPrism(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color)
    : Primitive(localTransform, color), mesh_(MeshRegistry::getInstance().getRectangularPrism()) {
}

void RectangularPrism::drawSolid() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(activeColor_.r_, activeColor_.g_, activeColor_.b_);

    glBegin(GL_QUADS);

    for (const auto& vertex : mesh_->vertices_) {
        glVertex3f(vertex.x_, vertex.y_, vertex.z_);
    }

    glEnd();
}

void RectangularPrism::drawEdges() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);;

    glBegin(GL_LINES);

    static constexpr int edgeVertexIndices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        9, 8, 12, 13, 15, 14, 10, 11
    };

    for (int index : edgeVertexIndices) {
        const auto& vertex = mesh_->vertices_[index];
        glVertex3f(vertex.x_, vertex.y_, vertex.z_);
    }

    glEnd();
}