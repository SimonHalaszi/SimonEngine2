#ifndef MESH_REGISTRY_HPP
#define MESH_REGISTRY_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "EngineUtil.hpp"
#include "EngineRender.hpp"

// Registry for Meshes
class MeshRegistry {
	public:
		static MeshRegistry& getInstance() {
			static MeshRegistry instance;
			return instance;
		}

		// All meshes are unit meshes.
		const EngineUtil::Mesh* getEdge();
		const EngineUtil::Mesh* getRectangularPrism();
		const EngineUtil::Mesh* getPlane(int subdivisions);
		const EngineUtil::Mesh* getCone(int slices);
		const EngineUtil::Mesh* getCylinder(int slices);
		const EngineUtil::Mesh* getEllipsoid(int slices, int stacks);
		const EngineUtil::Mesh* getDotObj(const std::string& filepath);

		const EngineRender::GPUMesh* getGPUMesh(const std::string& key);
		const EngineRender::GPUMesh* getOrCreateGPUMesh(const std::string& key, const EngineUtil::Mesh& CPUMesh);

		const EngineRender::GPUMesh* getGPUEdge();
		const EngineRender::GPUMesh* getGPURectangularPrism();
		const EngineRender::GPUMesh* getGPUPlane(int subdivisions);
		const EngineRender::GPUMesh* getGPUCone(int slices);
		const EngineRender::GPUMesh* getGPUCylinder(int slices);
		const EngineRender::GPUMesh* getGPUEllipsoid(int slices, int stacks);
		const EngineRender::GPUMesh* getGPUDotObj(const std::string& filepath);

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
		std::unordered_map<std::string, std::unique_ptr<EngineRender::GPUMesh>> GPURegistry_;

};

#endif