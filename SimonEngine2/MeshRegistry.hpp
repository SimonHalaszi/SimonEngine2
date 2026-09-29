#ifndef MESH_REGISTRY_HPP
#define MESH_REGISTRY_HPP

#include <GL/glut.h>
#include <GL/freeglut.h>

#include <unordered_map>
#include <iostream>
#include <string>

#include "EngineUtil.hpp"
#include "EngineMath.hpp"

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
		const EngineUtil::Mesh* getDotObj(std::string filepath);

		void clearRegistry() { std::cout << "MeshRegistry::clearRegistry : Cleared MeshRegistry\n"; registry_.clear(); }

		MeshRegistry(const MeshRegistry&) = delete;
		MeshRegistry& operator=(const MeshRegistry&) = delete;
		MeshRegistry(const MeshRegistry&&) = delete;
		MeshRegistry& operator=(const MeshRegistry&&) = delete;

	private:
		MeshRegistry() = default;

		~MeshRegistry() {}

		// Registry
		std::unordered_map<std::string, EngineUtil::Mesh> registry_;
};

#endif