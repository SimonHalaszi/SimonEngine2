#ifndef RECTANGULAR_PRISM_HPP
#define RECTANGULAR_PRISM_HPP

#include "Primitive.hpp"

class RectangularPrism : public Primitive {
    public:
        RectangularPrism() : RectangularPrism(EngineMath::Transform{}) {}
        RectangularPrism(const EngineMath::Transform& localTransform, const EngineRender::Material* material = nullptr);
    
    private:
        virtual const void resolveMeshes() override;
};

#endif 