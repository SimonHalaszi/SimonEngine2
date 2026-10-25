#ifndef OBJ_MESH_HPP
#define OBJ_MESH_HPP

#include "Primitive.hpp"

class ObjMesh : public Primitive {
    public:
        ObjMesh() : ObjMesh(EngineMath::Transform{}, EngineUtil::ColorRGB{}) {}
        ObjMesh(
            const EngineMath::Transform& localTransform,
            const EngineUtil::ColorRGB& color,
            const std::string& dotObjfilePath = "../Assets/null.obj",
            const std::string& vertPath = "../Shaders/Basic.vert",
            const std::string& fragPath = "../Shaders/Color.frag"
        );
    
    private:
        std::string filepath_;

        virtual const void resolveMeshes() override;
};

#endif
