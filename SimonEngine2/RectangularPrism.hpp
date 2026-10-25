#ifndef RECTANGULAR_PRISM_HPP
#define RECTANGULAR_PRISM_HPP

#include "Primitive.hpp"

class RectangularPrism : public Primitive {
    public:
        RectangularPrism() : RectangularPrism(EngineMath::Transform{}, EngineUtil::ColorRGB{}) {}
        RectangularPrism(
            const EngineMath::Transform& localTransform,
            const EngineUtil::ColorRGB& color,
            const std::string& vertPath = "../Shaders/Basic.vert",
            const std::string& fragPath = "../Shaders/Color.frag"
        );
    
    private:
        virtual const void resolveMeshes() override;
};

#endif 