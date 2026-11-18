#include "RectangularPrism.hpp"

RectangularPrism::RectangularPrism(const EngineMath::Transform& localTransform, const EngineRender::Material& material)
	: Primitive(localTransform, material) {}

const void RectangularPrism::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getRectangularPrism();
	GPUMesh_ = MeshRegistry::getInstance().getGPURectangularPrism();
}