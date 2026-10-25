#include "Plane.hpp"

Plane::Plane(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	int subdivisions,
	const std::string& vertPath,
	const std::string& fragPath
)
: Primitive(localTransform, color, vertPath, fragPath) {
	subdivisions_ = subdivisions;
}

const void Plane::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getPlane(subdivisions_);
	GPUMesh_ = MeshRegistry::getInstance().getGPUPlane(subdivisions_);
}