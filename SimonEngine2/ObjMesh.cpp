#include "ObjMesh.hpp"

#include "MeshRegistry.hpp"

ObjMesh::ObjMesh(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, const std::string& filepath)
    : Primitive(localTransform, color), mesh_(MeshRegistry::getInstance().getDotObj(filepath)) {
}

void ObjMesh::drawSolid() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(activeColor_.r_, activeColor_.g_, activeColor_.b_);

    glBegin(GL_TRIANGLES);

    for (std::size_t i = 0; i < mesh_->faceIndices_.size(); ++i) {
        const int index = mesh_->faceIndices_[i];
        const auto& vertex = mesh_->vertices_[index];

        glVertex3f(vertex.x_, vertex.y_, vertex.z_);
    }

    glEnd();
}

void ObjMesh::drawEdges() const {
    if (!mesh_) {
        return;
    }

    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);;
    
    glBegin(GL_LINES);

    for (std::size_t i = 0; i + 1 < mesh_->edgeIndices_.size(); i += 2) {
        const auto& first = mesh_->vertices_[mesh_->edgeIndices_[i]];
        const auto& second = mesh_->vertices_[mesh_->edgeIndices_[i + 1]];

        glVertex3f(first.x_, first.y_, first.z_);
        glVertex3f(second.x_, second.y_, second.z_);
    }

    glEnd();
}