#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <vector>

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Cylinder : public Primitive {
    public:
        Cylinder() = default;
        Cylinder(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices);

    private:
        float radius_ = 0.5f;
        float halfHeight_ = 0.5f;
        int slices_ = 16;

        void buildMesh();
        const EngineUtil::Vertex& vertexAt(int ring, int slice) const;

        std::vector<EngineUtil::Vertex> vertices_;

        void drawSolid() const override;
        void drawEdges() const override;
};

#endif