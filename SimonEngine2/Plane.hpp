#ifndef PLANE_HPP
#define PLANE_HPP

#include "Primitive.hpp"

class Plane : public Primitive {
    public:
        Plane() : Plane(EngineMath::Transform{}) {}
        Plane(const EngineMath::Transform& localTransform, const EngineRender::Material& material = {}, int subdivisions = 1);

    private:
        int subdivisions_;

        virtual const void resolveMeshes() override;
};

#endif 