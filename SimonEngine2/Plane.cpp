#include "Plane.hpp"

Plane::Plane(const EngineMath::Transform& localTransform, const EngineRender::Material* material, int subdivisions)
	: Primitive(localTransform, material), subdivisions_(subdivisions) {}

const void Plane::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getPlane(subdivisions_);
	GPUMesh_ = MeshRegistry::getInstance().getGPUPlane(subdivisions_);
}