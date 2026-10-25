#ifndef MESH_REGISTRY_HPP
#define MESH_REGISTRY_HPP

#include "GraphicsAPI.hpp"

#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream> 
#include <cstdint>
#include <iostream>
#include <string>
#include <cmath>

#include "EngineUtil.hpp"
#include "EngineMath.hpp"

#include "GPUMesh.hpp"
#include "GPUMeshUpload.hpp"

// Registry for Meshes
class MeshRegistry {
	public:
		static MeshRegistry& getInstance() {
			static MeshRegistry instance;
			return instance;
		}

		// All meshes are unit meshes. They are scaled, rotated, dynamically inside draw functions, but not altered

		const EngineUtil::Mesh* getCone(int slices);
		const EngineUtil::Mesh* getCylinder(int slices);
		const EngineUtil::Mesh* getRectangularPrism(); // Kinda useless but for consistent interface
		const EngineUtil::Mesh* getSphere(int slices, int stacks);
		const EngineUtil::Mesh* getDotObj(const std::string& filepath);

		const GPUMesh* getGPUSphere(int slices, int stacks);
		const GPUMesh* getGPUMesh(const std::string& key);
		const GPUMesh* getOrCreateGPUMesh(const std::string& key, const EngineUtil::Mesh& CPUMesh);

		void clearRegistry();

		MeshRegistry(const MeshRegistry&) = delete;
		MeshRegistry& operator=(const MeshRegistry&) = delete;
		MeshRegistry(const MeshRegistry&&) = delete;
		MeshRegistry& operator=(const MeshRegistry&&) = delete;

	private:
		MeshRegistry() = default;

		~MeshRegistry() {}

		// Registry
		std::unordered_map<std::string, EngineUtil::Mesh> registry_;
		std::unordered_map<std::string, std::unique_ptr<GPUMesh>> GPURegistry_;

};

#endif