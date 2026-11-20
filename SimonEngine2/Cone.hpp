#ifndef CONE_HPP
#define CONE_HPP

#include "Primitive.hpp"

class Cone : public Primitive {
    public:
        Cone() : Cone(EngineMath::Transform{}) {}
        Cone(const EngineMath::Transform& localTransform, const EngineRender::Material* material = nullptr, int slices = 3);

    private:
        int slices_;

        virtual const void resolveMeshes() override;
};

#endif