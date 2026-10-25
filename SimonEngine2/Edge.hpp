#ifndef EDGE_HPP
#define EDGE_HPP

#include "Primitive.hpp"

class Edge : public Primitive {
    public:
        Edge() : Edge(EngineMath::Transform{}, EngineUtil::ColorRGB{}) {}
        Edge(
            const EngineMath::Transform& localTransform,
            const EngineUtil::ColorRGB& color,
            const std::string& vertPath = "../Shaders/Basic.vert",
            const std::string& fragPath = "../Shaders/Color.frag"
        );

        void draw() const override;

    private:
        virtual const void resolveMeshes() override;
};

#endif
