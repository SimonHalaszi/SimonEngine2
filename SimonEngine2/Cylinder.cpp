#include "Cylinder.hpp"

Cylinder::Cylinder(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	int slices,
	const std::string& vertPath,
	const std::string& fragPath
)
: Primitive(localTransform, color, vertPath, fragPath) {
	slices_ = slices;
	mesh_ = MeshRegistry::getInstance().getCylinder(slices_);
	GPUMesh_ = MeshRegistry::getInstance().getGPUCylinder(slices_);
}