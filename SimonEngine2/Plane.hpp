#ifndef PLANE_HPP
#define PLANE_HPP

#include "Primitive.hpp"

class Plane : public Primitive {
    public:
        Plane() : Plane(EngineMath::Transform{}, EngineUtil::ColorRGB{}) {}
        Plane(
            const EngineMath::Transform& localTransform,
            const EngineUtil::ColorRGB& color,
            int subdivisions = 1,
            const std::string& vertPath = "../Shaders/Basic.vert",
            const std::string& fragPath = "../Shaders/Color.frag"
        );

    private:
        int subdivisions_;
};

#endif 