#include "Cone.hpp"

Cone::Cone(const EngineMath::Transform& localTransform, const EngineRender::Material* material, int slices)
    : Primitive(localTransform, material), slices_(slices) {}

const void Cone::resolveMeshes() {
    mesh_ = MeshRegistry::getInstance().getCone(slices_);
    GPUMesh_ = MeshRegistry::getInstance().getGPUCone(slices_);
}
