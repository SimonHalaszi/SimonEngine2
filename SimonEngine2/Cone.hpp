#ifndef CONE_HPP
#define CONE_HPP

#include "Primitive.hpp"

class Cone : public Primitive {
    public:
        Cone() : Cone(EngineMath::Transform{}, EngineUtil::ColorRGB{}) {}
        Cone(
            const EngineMath::Transform& localTransform, 
            const EngineUtil::ColorRGB& color, 
            int slices = 3, 
            const std::string& vertPath = "../Shaders/Basic.vert",
            const std::string& fragPath = "../Shaders/Color.frag"
        );

    private:
        int slices_;

        virtual const void resolveMeshes() override;
};

#endif