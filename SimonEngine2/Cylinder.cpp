#include "Cylinder.hpp"

#include "MeshRegistry.hpp"

Cylinder::Cylinder(const EngineMath::Transform& localTransform, const EngineRender::Material* material, int slices)
	: Primitive(localTransform, material), slices_(slices) {}

const void Cylinder::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getCylinder(slices_);
	GPUMesh_ = MeshRegistry::getInstance().getGPUCylinder(slices_);
}