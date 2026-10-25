#include "Edge.hpp"

Edge::Edge(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	const std::string& vertPath,
	const std::string& fragPath
)
: Primitive(localTransform, color, vertPath, fragPath) {}

const void Edge::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getEdge();
	GPUMesh_ = MeshRegistry::getInstance().getGPUEdge();
}

void Edge::draw() const {
	if (GPUMesh_ == nullptr) {
		return;
	}

	if (drawAsSolid_) {
		Renderer::getInstance().drawMesh(
			*GPUMesh_,
			material_,
			getWorldMatrix(),
			Renderer::DrawMode::Edges
		);
	}
	else {
		Renderer::getInstance().drawMesh(
			*GPUMesh_,
			edgeMaterial_,
			getWorldMatrix(),
			Renderer::DrawMode::Edges
		);
	}
}