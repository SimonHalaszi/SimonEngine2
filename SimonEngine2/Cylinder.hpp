#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include "Primitive.hpp"

class Cylinder : public Primitive {
    public:
        Cylinder() : Cylinder(EngineMath::Transform{}) {}
        Cylinder(const EngineMath::Transform& localTransform, const EngineRender::Material& material = {}, int slices = 3);

    private:
        int slices_;

        virtual const void resolveMeshes() override;
};

#endif