#include "Edge.hpp"

Edge::Edge(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	const std::string& vertPath,
	const std::string& fragPath
)
: Primitive(localTransform, color, vertPath, fragPath) {
	mesh_ = MeshRegistry::getInstance().getEdge();
	GPUMesh_ = MeshRegistry::getInstance().getGPUEdge();
}