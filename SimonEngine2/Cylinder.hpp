#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <algorithm>

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Cylinder : public Primitive {
    public:
        Cylinder() = default;
        Cylinder(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices);

    private:
        int slices_ = 16;
        const EngineUtil::Vertex& vertexAt(int ring, int slice) const;
        const EngineUtil::Mesh* mesh_ = nullptr;

        void drawSolid() const override;
        void drawEdges() const override;
};

#endif