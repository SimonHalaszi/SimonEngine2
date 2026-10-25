#include "Cone.hpp"

Cone::Cone(
    const EngineMath::Transform& localTransform,
    const EngineUtil::ColorRGB& color,
    int slices,
    const std::string& vertPath,
    const std::string& fragPath
) 
: Primitive(localTransform, color, vertPath, fragPath) {
    slices_ = slices;
    mesh_ = MeshRegistry::getInstance().getCone(slices_);
    GPUMesh_ = MeshRegistry::getInstance().getGPUCone(slices_);
}
