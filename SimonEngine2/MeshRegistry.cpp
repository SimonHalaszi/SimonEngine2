#include "MeshRegistry.hpp"

namespace {
	constexpr float unit = 1.0f;
	constexpr float halfUnit = unit / 2.0f;
	constexpr int minSlices = 3;
}

const EngineUtil::Mesh* MeshRegistry::getCone(int slices) {
	if (slices < minSlices) {
		slices = minSlices;
	}
	
	// Generated Key
	std::string key = "SE2_MR_CONE_" + slices;

	EngineUtil::Mesh mesh;
	
	for (int slice = 0; slice <= slices; ++slice) {
		const float theta =
			2.0f * EngineMath::PI * static_cast<float>(slice) / slices;

		const float x = halfUnit * std::cos(theta);
		const float z = halfUnit * std::sin(theta);

		mesh.vertices_.push_back({ x, halfUnit, z });
	}

	registry_.insert({ key, mesh });
}

const EngineUtil::Mesh* MeshRegistry::getCylinder(int slices) {
	if (slices < minSlices) {
		slices = minSlices;
	}

	// Generated Key
	std::string key = "SE2_MR_CYLIN_" + slices;

	EngineUtil::Mesh mesh;

	for (int ring = 0; ring < 2; ++ring) {
		const float y = (ring == 0) ? -halfUnit : halfUnit;

		for (int slice = 0; slice <= slices; ++slice) {
			const float theta =
				2.0f * EngineMath::PI * static_cast<float>(slice) / slices;

			const float x = halfUnit * std::cos(theta);
			const float z = halfUnit * std::sin(theta);

			mesh.vertices_.push_back({ x, y, z });
		}
	}

	registry_.insert({ key, mesh });
}

const EngineUtil::Mesh* MeshRegistry::getRectangularPrism() {
	// Generated Key
	std::string key = "SE2_MR_RPRISM";

	EngineUtil::Mesh mesh;

	// Take code from already existing primitives

	registry_.insert({ key, mesh });
}

const EngineUtil::Mesh* MeshRegistry::getSphere(int slices, int stacks) {
	// Generated Key
	std::string key = (("SE2_MR_SPHERE_" + slices) + stacks);

	EngineUtil::Mesh mesh;

	for (int stack = 0; stack <= stacks; ++stack) {
		const float phi = EngineMath::PI * static_cast<float>(stack) / stacks;

		for (int slice = 0; slice <= slices; ++slice) {
			const float theta = 2.0f * EngineMath::PI * static_cast<float>(slice) / slices;

			const float x = std::sin(phi) * std::cos(theta);
			const float y = std::cos(phi);
			const float z = std::sin(phi) * std::sin(theta);

			mesh.vertices_.push_back({ halfUnit * x, halfUnit * y, halfUnit * z });
		}
	}

	registry_.insert({ key, mesh });
}

const EngineUtil::Mesh* MeshRegistry::getDotObj(std::string filepath) {
	// Generated Key
	std::string key = "SE2_MR_OBJ_" + filepath;

	EngineUtil::Mesh mesh;

	// Take code from already existing primitives

	registry_.insert({ key, mesh });
}
