#ifndef CONE_HPP
#define CONE_HPP

#include <vector>

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Cone : public Primitive {
    public:
        Cone() = default;
        Cone(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices);

    private:
        float radius_ = 0.5f;
        float halfHeight_ = 0.5f;
        int slices_ = 16;

        void buildMesh();

        std::vector<EngineUtil::Vertex> vertices_;

        void drawSolid() const override;
        void drawEdges() const override;
};

#endif