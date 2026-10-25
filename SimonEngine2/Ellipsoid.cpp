#include "Ellipsoid.hpp"

#include "MeshRegistry.hpp"

Ellipsoid::Ellipsoid(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	int slices,
	int stacks,
	const std::string& vertPath,
	const std::string& fragPath
)
: Primitive(localTransform, color, vertPath, fragPath) {
	slices_ = slices;
	stacks_ = stacks;
}

const void Ellipsoid::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getEllipsoid(slices_, stacks_);
	GPUMesh_ = MeshRegistry::getInstance().getGPUEllipsoid(slices_, stacks_);
}