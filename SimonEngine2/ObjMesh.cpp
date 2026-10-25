#include "ObjMesh.hpp"

ObjMesh::ObjMesh(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	const std::string& dotObjfilePath,
	const std::string& vertPath,
	const std::string& fragPath
)
: Primitive(localTransform, color, vertPath, fragPath) {
	mesh_ = MeshRegistry::getInstance().getDotObj(dotObjfilePath);
	GPUMesh_ = MeshRegistry::getInstance().getGPUDotObj(dotObjfilePath);
}