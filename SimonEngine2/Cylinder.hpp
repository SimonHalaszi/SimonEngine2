#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include "Primitive.hpp"

class Cylinder : public Primitive {
    public:
        Cylinder() : Cylinder(EngineMath::Transform{}, EngineUtil::ColorRGB{}) {}
        Cylinder(
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