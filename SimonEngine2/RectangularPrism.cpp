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

    // 24 vertices = 6 faces x 4 corners. The prism is centered on the origin, so the
    // average of a face's four corners IS that face's center, and the direction from
    // the origin to a face center is exactly its outward normal. No winding-order
    // assumptions, no cross products, can't get it backwards. Big brain.
    const auto& v = mesh_->vertices_;
    for (std::size_t face = 0; face + 3 < v.size(); face += 4) {
        const EngineMath::Vector3 normal = EngineMath::Vector3(
            v[face].x_ + v[face + 1].x_ + v[face + 2].x_ + v[face + 3].x_,
            v[face].y_ + v[face + 1].y_ + v[face + 2].y_ + v[face + 3].y_,
            v[face].z_ + v[face + 1].z_ + v[face + 2].z_ + v[face + 3].z_
        ).normalized();

        glNormal3f(normal.x_, normal.y_, normal.z_);

        for (std::size_t corner = face; corner < face + 4; ++corner) {
            glVertex3f(v[corner].x_, v[corner].y_, v[corner].z_);
        }
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