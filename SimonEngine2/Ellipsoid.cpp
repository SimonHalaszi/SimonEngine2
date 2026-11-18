#include "Ellipsoid.hpp"

#include "MeshRegistry.hpp"

Ellipsoid::Ellipsoid(const EngineMath::Transform& localTransform, const EngineRender::Material& material, int slices, int stacks)
	: Primitive(localTransform, material), slices_(slices), stacks_(stacks) {}

const void Ellipsoid::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getEllipsoid(slices_, stacks_);
	GPUMesh_ = MeshRegistry::getInstance().getGPUEllipsoid(slices_, stacks_);
}