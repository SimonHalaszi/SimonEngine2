#ifndef CONE_HPP
#define CONE_HPP

#include <vector>

#include "Object.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Cone : public Object {
    public:
        Cone() = default;
        Cone(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices);

        void draw() override;
        void update() override;

    private:
        bool drawAsSolid_ = false;
        float radius_ = 0.5f;
        float halfHeight_ = 0.5f;
        int slices_ = 16;

        void buildMesh();

        std::vector<EngineUtil::Vertex> vertices_;

        void drawSolid() const;
        void drawEdges() const;

        EngineUtil::ColorRGB color_;
};

#endif