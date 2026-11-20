#include "ObjMesh.hpp"

ObjMesh::ObjMesh(const EngineMath::Transform& localTransform, const EngineRender::Material* material, const std::string& dotObjfilePath)
	: Primitive(localTransform, material), filepath_(dotObjfilePath) {}

const void ObjMesh::resolveMeshes() {
	mesh_ = MeshRegistry::getInstance().getDotObj(filepath_);
	GPUMesh_ = MeshRegistry::getInstance().getGPUDotObj(filepath_);
}