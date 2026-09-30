#include "MeshRegistry.hpp"

namespace {
	constexpr float unit = 1.0f;
	constexpr float halfUnit = unit / 2.0f;
	constexpr int minSlices = 3;
	constexpr int minStacks = 3;

	// For converting obj style indices
	int resolveObjIndex(int objIndex, int count) {
		if (objIndex > 0) {
			return objIndex - 1;
		}

		if (objIndex < 0) {
			return count + objIndex;
		}

		return -1;
	}

	bool parsePositionIndex(const std::string& token, int positionCount, int& resolvedIndex) {
		const std::string positionPart = token.substr(0, token.find('/'));

		if (positionPart.empty()) {
			return false;
		}

		const int objIndex = std::stoi(positionPart);
		resolvedIndex = resolveObjIndex(objIndex, positionCount);

		return resolvedIndex >= 0 && resolvedIndex < positionCount;
	}

	// This function exist so that edges are not doubly added
	// Using a unsigned 64 int to hold two ints in order, the lower int in the first 32 bits, the upper int in the second 32 bits
	std::uint64_t makeEdgeKey(int first, int second) {
		const int lower = std::min(first, second);
		const int upper = std::max(first, second);

		// Bit shifting over the lower int 32 bits to the left, bit wise or so that the upper int is put in the remaining 32 bits
		return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(lower)) << 32) | static_cast<std::uint32_t>(upper);
	}

	// Function adds edges to given mesh, ensures no duplicates. Needs an unordered set of unique edges to check against.
	void addEdge(EngineUtil::Mesh& mesh, std::unordered_set<std::uint64_t>& uniqueEdges, int first, int second) {
		const std::uint64_t key = makeEdgeKey(first, second);

		if (!uniqueEdges.insert(key).second) {
			return;
		}

		mesh.edgeIndices_.push_back(first);
		mesh.edgeIndices_.push_back(second);
	}
}

const EngineUtil::Mesh* MeshRegistry::getCone(int slices) {
	if (slices < minSlices) {
		slices = minSlices;
	}

	const std::string key = "SE2_MR_CONE_" + std::to_string(slices);

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	EngineUtil::Mesh mesh;

	for (int slice = 0; slice <= slices; ++slice) {
		const float theta =
			2.0f * EngineMath::PI * static_cast<float>(slice) / slices;

		const float x = halfUnit * std::cos(theta);
		const float z = halfUnit * std::sin(theta);

		mesh.vertices_.push_back({ x, -halfUnit, z });
	}

	auto inserted = registry_.insert({ key, std::move(mesh) });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getCylinder(int slices) {
	if (slices < minSlices) {
		slices = minSlices;
	}

	const std::string key = "SE2_MR_CYLIN_" + std::to_string(slices);

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

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

	auto inserted = registry_.insert({ key, std::move(mesh) });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getRectangularPrism() {
	const std::string key = "SE2_MR_RPRISM";

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	EngineUtil::Mesh mesh;

	const float hx = halfUnit;
	const float hy = halfUnit;
	const float hz = halfUnit;

	mesh.vertices_.push_back({ -hx, -hy, hz });
	mesh.vertices_.push_back({ hx, -hy, hz });
	mesh.vertices_.push_back({ hx, hy, hz });
	mesh.vertices_.push_back({ -hx, hy, hz });

	mesh.vertices_.push_back({ hx, -hy, -hz });
	mesh.vertices_.push_back({ -hx, -hy, -hz });
	mesh.vertices_.push_back({ -hx, hy, -hz });
	mesh.vertices_.push_back({ hx, hy, -hz });

	mesh.vertices_.push_back({ -hx, -hy, -hz });
	mesh.vertices_.push_back({ -hx, -hy, hz });
	mesh.vertices_.push_back({ -hx, hy, hz });
	mesh.vertices_.push_back({ -hx, hy, -hz });

	mesh.vertices_.push_back({ hx, -hy, hz });
	mesh.vertices_.push_back({ hx, -hy, -hz });
	mesh.vertices_.push_back({ hx, hy, -hz });
	mesh.vertices_.push_back({ hx, hy, hz });

	mesh.vertices_.push_back({ -hx, hy, hz });
	mesh.vertices_.push_back({ hx, hy, hz });
	mesh.vertices_.push_back({ hx, hy, -hz });
	mesh.vertices_.push_back({ -hx, hy, -hz });

	mesh.vertices_.push_back({ -hx, -hy, -hz });
	mesh.vertices_.push_back({ hx, -hy, -hz });
	mesh.vertices_.push_back({ hx, -hy, hz });
	mesh.vertices_.push_back({ -hx, -hy, hz });

	auto inserted = registry_.insert({ key, std::move(mesh) });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getSphere(int slices, int stacks) {
	if (slices < minSlices) {
		slices = minSlices;
	}
	if (stacks < minStacks) {
		stacks = minStacks;
	}

	const std::string key = "SE2_MR_SPHERE_" + std::to_string(slices) + "_" + std::to_string(stacks);

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

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

	auto inserted = registry_.insert({ key, std::move(mesh) });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getDotObj(const std::string& filepath) {
	const std::string key = "SE2_MR_OBJ_" + filepath;

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	std::ifstream file(filepath);
	if (!file) {
		return nullptr;
	}

	EngineUtil::Mesh mesh;
	std::unordered_set<std::uint64_t> uniqueEdges;

	std::string line;

	while (std::getline(file, line)) {
		std::istringstream stream(line);
		std::string type;

		stream >> type;

		if (type.empty() || type[0] == '#') {
			continue;
		}

		// Handle Vertice Reading
		if (type == "v") {
			float x;
			float y;
			float z;

			if (stream >> x >> y >> z) {
				mesh.vertices_.push_back({ x, y, z });
			}

			continue;
		}

		// Handle Face Reading
		if (type == "f") {
			std::vector<int> polygon;
			std::string token;

			while (stream >> token) {
				int vertexIndex = -1;
				if (!parsePositionIndex(token, static_cast<int>(mesh.vertices_.size()), vertexIndex)) {
					polygon.clear();
					break;
				}
				polygon.push_back(vertexIndex);
			}

			if (polygon.size() < 3) {
				continue;
			}

			for (std::size_t i = 1; i + 1 < polygon.size(); ++i) {
				const int first = polygon[0];
				const int second = polygon[i];
				const int third = polygon[i + 1];

				// Add face indices
				mesh.faceIndices_.push_back(first);
				mesh.faceIndices_.push_back(second);
				mesh.faceIndices_.push_back(third);

				// Add edges
				addEdge(mesh, uniqueEdges, first, second);
				addEdge(mesh, uniqueEdges, second, third);
				addEdge(mesh, uniqueEdges, third, first);
			}
		}
	}

	// Nothing was actually created
	if (mesh.vertices_.empty() || mesh.faceIndices_.empty()) {
		return nullptr;
	}

	auto inserted = registry_.insert({ key, std::move(mesh) });
	return &inserted.first->second;
}