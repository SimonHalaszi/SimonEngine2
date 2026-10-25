#include "RectangularPrism.hpp"

RectangularPrism::RectangularPrism(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	const std::string& vertPath,
	const std::string& fragPath
)
: Primitive(localTransform, color, vertPath, fragPath) {}

const void RectangularPrism::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getRectangularPrism();
	GPUMesh_ = MeshRegistry::getInstance().getGPURectangularPrism();
}