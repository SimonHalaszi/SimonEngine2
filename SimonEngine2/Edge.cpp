#include "Edge.hpp"

Edge::Edge(const EngineMath::Transform& localTransform, const EngineRender::Material* material)
	: Primitive(localTransform, material) {}

const void Edge::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getEdge();
	GPUMesh_ = MeshRegistry::getInstance().getGPUEdge();
}

void Edge::draw() const {
	if (GPUMesh_ == nullptr) {
		return;
	}

	if (drawAsSolid_) {
		Renderer::getInstance().submitMesh(
			*GPUMesh_,
			*material_,
			getWorldMatrix(),
			Renderer::DrawMode::Edges
		);
	}
	else {
		Renderer::getInstance().submitMesh(
			*GPUMesh_,
			*edgeMaterial_,
			getWorldMatrix(),
			Renderer::DrawMode::Edges
		);
	}
}