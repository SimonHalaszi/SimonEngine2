#ifndef EDGE_HPP
#define EDGE_HPP

#include "Primitive.hpp"

class Edge : public Primitive {
    public:
        Edge() : Edge(EngineMath::Transform{}) {}
        Edge(const EngineMath::Transform& localTransform, const EngineRender::Material& material = {});

        void draw() const override;

    private:
        virtual const void resolveMeshes() override;
};

#endif
