#ifndef GPU_MESH_UPLOAD_HPP
#define GPU_MESH_UPLOAD_HPP

#include "EngineUtil.hpp"
#include "GPUMesh.hpp"

#include <stdexcept>

namespace GPUMeshUpload {
	// Uploads CPU Mesh data into an existing GPUMesh object
	void upload(const EngineUtil::Mesh& CPUMesh, GPUMesh& outGPUMesh);
}

#endif