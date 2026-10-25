#include "ObjMesh.hpp"

ObjMesh::ObjMesh(
	const EngineMath::Transform& localTransform,
	const EngineUtil::ColorRGB& color,
	const std::string& dotObjfilePath,
	const std::string& vertPath,
	const std::string& fragPath
)
: Primitive(localTransform, color, vertPath, fragPath) {
	filepath_ = dotObjfilePath;
}

const void ObjMesh::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getDotObj(filepath_);
	GPUMesh_ = MeshRegistry::getInstance().getGPUDotObj(filepath_);
}