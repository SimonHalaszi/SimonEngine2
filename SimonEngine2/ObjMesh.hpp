#ifndef OBJ_MESH_HPP
#define OBJ_MESH_HPP

#include <string>

#include "Primitive.hpp"

class ObjMesh : public Primitive {
    public:
        ObjMesh() : ObjMesh(EngineMath::Transform{}) {}
        ObjMesh(const EngineMath::Transform& localTransform, const EngineRender::Material* material = nullptr, const std::string& dotObjfilePath = "../Assets/null.obj");
    
    private:
        std::string filepath_;

        virtual const void resolveMeshes() override;
};

#endif
