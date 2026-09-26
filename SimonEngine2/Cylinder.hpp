#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <vector>

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Cylinder : public Object {
    public:
        Cylinder() = default;
        Cylinder(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices);

        void draw() override;
        void update() override;

    private:
        bool drawAsSolid_ = true;
        float radius_ = 0.5f;
        float halfHeight_ = 0.5f;
        int slices_ = 16;

        void buildMesh();
        const EngineUtil::Vertex& vertexAt(int ring, int slice) const;

        std::vector<EngineUtil::Vertex> vertices_;

        void drawSolid() const;
        void drawEdges() const;

        bool drawColor_ = true;
        EngineUtil::ColorRGB activeColor_;
        EngineUtil::ColorRGB color_;
        EngineUtil::ColorRGB grayColor_;
};

#endif