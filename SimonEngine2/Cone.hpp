#ifndef CONE_HPP
#define CONE_HPP

#include <algorithm>

#include "Primitive.hpp"
#include "EngineMath.hpp"
#include "EngineUtil.hpp"

class Cone : public Primitive {
    public:
        Cone() = default;
        Cone(const EngineMath::Transform& localTransform, const EngineUtil::ColorRGB& color, int slices);

    private:
        int slices_ = 16;
        const EngineUtil::Mesh* mesh_ = nullptr;

        void drawSolid() const override;
        void drawEdges() const override;
};

#endif