#include "MeshRegistry.hpp"

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#include "EngineMath.hpp"

namespace {
	constexpr float unit = 1.0f;
	constexpr float halfUnit = unit * 0.5f;

	constexpr int minSubdivisions = 1;
	constexpr int minSlices = 3;
	constexpr int minStacks = 2;

	// This function exist so that edges are not doubly added
	// Using a unsigned 64 int to hold two ints in order, the lower int in the first 32 bits, the upper int in the second 32 bits
	// This is kind of a hacky work around because unorderd_set can not hash pairs. So alas we are using this to make a pair of ints
	std::uint64_t makeEdgeKey(int first, int second) {
		const int lower = std::min(first, second);
		const int upper = std::max(first, second);

		// Bit shifting over the lower int 32 bits to the left, bit wise or so that the upper int is put in the remaining 32 bits
		return (static_cast<std::uint64_t>(static_cast<std::uint32_t>(lower)) << 32) | static_cast<std::uint32_t>(upper);
	}

	void addEdgeUnique(EngineUtil::Mesh& mesh, std::unordered_set<std::uint64_t>& uniqueEdges, unsigned int first, unsigned int second) {
		if (first == second) {
			return;
		}

		const std::uint64_t key = makeEdgeKey(first, second);

		if (!uniqueEdges.insert(key).second) {
			return;
		}

		mesh.edgeIndices_.push_back(first);
		mesh.edgeIndices_.push_back(second);
	}

	EngineUtil::Mesh buildEdge() {
		EngineUtil::Mesh mesh;

		mesh.vertices_.push_back({ -halfUnit,0.0f,0.0f });
		mesh.vertices_.push_back({ halfUnit, 0.0f, 0.0f });

		mesh.edgeIndices_ = { 0u, 1u };

		return mesh;
	}

	EngineUtil::Mesh buildRectangularPrism() {
		EngineUtil::Mesh mesh;

		mesh.vertices_ = {
			{ -halfUnit, -halfUnit,  halfUnit }, // 0
			{  halfUnit, -halfUnit,  halfUnit }, // 1
			{  halfUnit,  halfUnit,  halfUnit }, // 2
			{ -halfUnit,  halfUnit,  halfUnit }, // 3
			{ -halfUnit, -halfUnit, -halfUnit }, // 4
			{  halfUnit, -halfUnit, -halfUnit }, // 5
			{  halfUnit,  halfUnit, -halfUnit }, // 6
			{ -halfUnit,  halfUnit, -halfUnit }  // 7
		};

		mesh.faceIndices_ = {
			// Front
			0u, 1u, 2u,  0u, 2u, 3u,
			// Back
			5u, 4u, 7u,  5u, 7u, 6u,
			// Left
			4u, 0u, 3u,  4u, 3u, 7u,
			// Right
			1u, 5u, 6u,  1u, 6u, 2u,
			// Top
			3u, 2u, 6u,  3u, 6u, 7u,
			// Bottom
			4u, 5u, 1u,  4u, 1u, 0u
		};

		mesh.edgeIndices_ = {
			0u, 1u, 1u, 2u, 2u, 3u, 3u, 0u,
			4u, 5u, 5u, 6u, 6u, 7u, 7u, 4u,
			0u, 4u, 1u, 5u, 2u, 6u, 3u, 7u
		};

		return mesh;
	}

	EngineUtil::Mesh buildPlane(int subdivisions) {
		if (subdivisions < minSubdivisions) {
			subdivisions = minSubdivisions;
		}

		EngineUtil::Mesh mesh;

		const int grid = subdivisions;
		const int verticesPerSide = grid + 1;

		mesh.vertices_.reserve(static_cast<std::size_t>(verticesPerSide * verticesPerSide));

		for (int z = 0; z <= grid; ++z) {
			const float vz = -halfUnit + unit * (static_cast<float>(z) / static_cast<float>(grid));

			for (int x = 0; x <= grid; ++x) {
				const float vx = -halfUnit + unit * (static_cast<float>(x) / static_cast<float>(grid));
				mesh.vertices_.push_back({ vx, 0.0f, vz });
			}
		}

		auto idx = [verticesPerSide](int x, int z)->unsigned int {
			return static_cast<unsigned int>(z * verticesPerSide + x);
		};

		for (int z = 0; z < grid; ++z) {
			for (int x = 0; x < grid; ++x) {
				const unsigned int a = idx(x, z);
				const unsigned int b = idx(x + 1, z);
				const unsigned int c = idx(x + 1, z + 1);
				const unsigned int d = idx(x, z + 1);

				mesh.faceIndices_.insert(mesh.faceIndices_.end(), { a, b, c, a, c, d });
			}
		}

		std::unordered_set<std::uint64_t> uniqueEdges;
		uniqueEdges.reserve(static_cast<std::size_t>(grid * (grid + 1) * 2));

		for (int z = 0; z <= grid; ++z) {
			for (int x = 0; x < grid; ++x) {
				addEdgeUnique(mesh, uniqueEdges, idx(x, z), idx(x + 1, z));
			}
		}
		for (int z = 0; z < grid; ++z) {
			for (int x = 0; x <= grid; ++x) {
				addEdgeUnique(mesh, uniqueEdges, idx(x, z), idx(x, z + 1));
			}
		}

		return mesh;
	}

	EngineUtil::Mesh buildCone(int slices) {
		if (slices < minSlices) {
			slices = minSlices;
		}

		EngineUtil::Mesh mesh;
		mesh.vertices_.reserve(static_cast<std::size_t>(slices + 2));

		for (int slice = 0; slice < slices; ++slice) {
			const float theta = 2.0f * EngineMath::PI * static_cast<float>(slice) / static_cast<float>(slices);
			const float x = halfUnit * std::cos(theta);
			const float z = halfUnit * std::sin(theta);
			mesh.vertices_.push_back({ x, -halfUnit, z });
		}

		const unsigned int tip = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, halfUnit, 0.0f });

		const unsigned int baseCenter = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, -halfUnit, 0.0f });

		for (int slice = 0; slice < slices; ++slice) {
			const unsigned int current = static_cast<unsigned int>(slice);
			const unsigned int next = static_cast<unsigned int>((slice + 1) % slices);

			// Side
			mesh.faceIndices_.insert(mesh.faceIndices_.end(), { current, tip, next });
			// Bottom cap
			mesh.faceIndices_.insert(mesh.faceIndices_.end(), { baseCenter, current, next });

			mesh.edgeIndices_.insert(mesh.edgeIndices_.end(), { current, next, current, tip });
		}

		return mesh;
	}

	EngineUtil::Mesh buildCylinder(int slices) {
		if (slices < minSlices) {
			slices = minSlices;
		}

		EngineUtil::Mesh mesh;
		mesh.vertices_.reserve(static_cast<std::size_t>(2 * slices + 2));

		// Bottom ring
		for (int slice = 0; slice < slices; ++slice) {
			const float theta = 2.0f * EngineMath::PI * static_cast<float>(slice) / static_cast<float>(slices);
			mesh.vertices_.push_back({ halfUnit * std::cos(theta), -halfUnit, halfUnit * std::sin(theta) });
		}
		// Top ring
		for (int slice = 0; slice < slices; ++slice) {
			const float theta = 2.0f * EngineMath::PI * static_cast<float>(slice) / static_cast<float>(slices);
			mesh.vertices_.push_back({ halfUnit * std::cos(theta), halfUnit, halfUnit * std::sin(theta) });
		}

		const unsigned int bottomCenter = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, -halfUnit, 0.0f });

		const unsigned int topCenter = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, halfUnit, 0.0f });

		for (int slice = 0; slice < slices; ++slice) {
			const unsigned int next = static_cast<unsigned int>((slice + 1) % slices);

			const unsigned int b0 = static_cast<unsigned int>(slice);
			const unsigned int b1 = next;
			const unsigned int t0 = static_cast<unsigned int>(slice + slices);
			const unsigned int t1 = static_cast<unsigned int>(next + slices);

			// Side
			mesh.faceIndices_.insert(mesh.faceIndices_.end(), { b0, t0, b1, b1, t0, t1 });
			// Bottom cap
			mesh.faceIndices_.insert(mesh.faceIndices_.end(), { bottomCenter, b0, b1 });
			// Top cap
			mesh.faceIndices_.insert(mesh.faceIndices_.end(), { topCenter, t1, t0 });

			mesh.edgeIndices_.insert(mesh.edgeIndices_.end(), { b0, b1, t0, t1, b0, t0 });
		}

		return mesh;
	}

	EngineUtil::Mesh buildEllipsoid(int slices, int stacks) {
		if (slices < minSlices) {
			slices = minSlices;
		}
		if (stacks < minStacks) {
			stacks = minStacks;
		}

		EngineUtil::Mesh mesh;

		const unsigned int southPole = 0u;
		mesh.vertices_.push_back({ 0.0f, -halfUnit, 0.0f });

		const int ringCount = stacks - 1; // excludes poles
		const int vertsPerRing = slices;

		for (int ring = 1; ring <= ringCount; ++ring) {
			const float v = static_cast<float>(ring) / static_cast<float>(stacks);
			const float phi = EngineMath::PI * v;

			const float y = halfUnit * std::cos(phi);
			const float r = halfUnit * std::sin(phi);

			for (int slice = 0; slice < slices; ++slice) {
				const float theta = 2.0f * EngineMath::PI * static_cast<float>(slice) / static_cast<float>(slices);
				const float x = r * std::cos(theta);
				const float z = r * std::sin(theta);
				mesh.vertices_.push_back({ x, y, z });
			}
		}

		const unsigned int northPole = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, halfUnit, 0.0f });

		auto ringVertex = [vertsPerRing](int ringIndex, int sliceIndex) -> unsigned int {
			const int wrappedSlice = (sliceIndex % vertsPerRing + vertsPerRing) % vertsPerRing;
			return static_cast<unsigned int>(1 + ringIndex * vertsPerRing + wrappedSlice);
			};

		const int lastRing = ringCount - 1;

		// North cap
		for (int slice = 0; slice < slices; ++slice) {
			const unsigned int a = ringVertex(0, slice);
			const unsigned int b = ringVertex(0, slice + 1);

			mesh.faceIndices_.insert(
				mesh.faceIndices_.end(),
				{ northPole, b, a }
			);
		}

		// Middle bands
		for (int ring = 0; ring < lastRing; ++ring) {
			for (int slice = 0; slice < slices; ++slice) {
				const unsigned int a = ringVertex(ring, slice);
				const unsigned int b = ringVertex(ring, slice + 1);
				const unsigned int c = ringVertex(ring + 1, slice + 1);
				const unsigned int d = ringVertex(ring + 1, slice);

				mesh.faceIndices_.insert(
					mesh.faceIndices_.end(),
					{ a, b, d, b, c, d }
				);
			}
		}

		// South cap
		for (int slice = 0; slice < slices; ++slice) {
			const unsigned int a = ringVertex(lastRing, slice);
			const unsigned int b = ringVertex(lastRing, slice + 1);

			mesh.faceIndices_.insert(
				mesh.faceIndices_.end(),
				{ southPole, a, b }
			);
		}

		for (int slice = 0; slice < slices; ++slice) {
			mesh.edgeIndices_.insert(
				mesh.edgeIndices_.end(),
				{ northPole, ringVertex(0, slice) }
			);

			for (int ring = 0; ring < lastRing; ++ring) {
				mesh.edgeIndices_.insert(
					mesh.edgeIndices_.end(),
					{
						ringVertex(ring, slice),
						ringVertex(ring + 1, slice)
					}
				);
			}

			mesh.edgeIndices_.insert(
				mesh.edgeIndices_.end(),
				{ ringVertex(lastRing, slice), southPole }
			);
		}

		for (int ring = 0; ring < ringCount; ++ring) {
			for (int slice = 0; slice < slices; ++slice) {
				mesh.edgeIndices_.insert(
					mesh.edgeIndices_.end(),
					{
						ringVertex(ring, slice),
						ringVertex(ring, slice + 1)
					}
				);
			}
		}

		return mesh;
	}

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
		resolvedIndex = -1;

		const std::string positionPart = token.substr(0, token.find('/'));

		if (positionPart.empty()) {
			return false;
		}

		int objIndex = 0;

		try {
			std::size_t consumed = 0;
			objIndex = std::stoi(positionPart, &consumed);

			if (consumed != positionPart.size()) {
				return false;
			}
		}
		catch (const std::invalid_argument&) {
			return false;
		}
		catch (const std::out_of_range&) {
			return false;
		}

		resolvedIndex = resolveObjIndex(objIndex, positionCount);

		return resolvedIndex >= 0 && resolvedIndex < positionCount;
	}

	EngineUtil::Mesh buildDotObj(const std::string& filepath) {
		std::ifstream file(filepath);

		if (!file) {
			throw std::runtime_error("MeshRegistry::getDotObj: failed to open file: " + filepath);
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

			if (type == "v") {
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;

				if (stream >> x >> y >> z) {
					mesh.vertices_.push_back({ x, y, z });
				}

				continue;
			}

			if (type == "f") {
				std::vector<unsigned int> polygon;
				std::string token;

				while (stream >> token) {
					if (!token.empty() && token[0] == '#') {
						break;
					}

					int vertexIndex = -1;
					if (!parsePositionIndex(token, static_cast<int>(mesh.vertices_.size()), vertexIndex)) {
						polygon.clear();
						break;
					}
					polygon.push_back(static_cast<unsigned int>(vertexIndex));
				}

				if (polygon.size() < 3) {
					continue;
				}

				// Boundary edges from original polygon.
				for (std::size_t i = 0; i < polygon.size(); ++i) {
					const unsigned int first = polygon[i];
					const unsigned int second = polygon[(i + 1) % polygon.size()];
					addEdgeUnique(mesh, uniqueEdges, first, second);
				}

				// Triangle fan
				for (std::size_t i = 1; i + 1 < polygon.size(); ++i) {
					mesh.faceIndices_.push_back(polygon[0]);
					mesh.faceIndices_.push_back(polygon[i]);
					mesh.faceIndices_.push_back(polygon[i + 1]);
				}
			}
		}

		if (mesh.vertices_.empty()) {
			throw std::runtime_error("MeshRegistry::getDotObj: file contains no vertices: " + filepath);
		}

		if (mesh.faceIndices_.empty()) {
			throw std::runtime_error("MeshRegistry::getDotObj: file contains no valid faces: " + filepath);
		}

		return mesh;
	}

	std::string makePlaneKey(int subdivisions) {
		return "SE2_MR_PLANE_" + std::to_string(subdivisions);
	}

	std::string makeConeKey(int slices) {
		return "SE2_MR_CONE_" + std::to_string(slices);
	}

	std::string makeCylinderKey(int slices) {
		return "SE2_MR_CYLINDER_" + std::to_string(slices);
	}

	std::string makeEllipsoidKey(int slices, int stacks) {
		return "SE2_MR_ELLIPSOID_" + std::to_string(slices) + "_" + std::to_string(stacks);
	}

	std::string makeObjKey(const std::string& filepath) {
		return "SE2_MR_OBJ_" + filepath;
	}
}

const EngineUtil::Mesh* MeshRegistry::getEdge() {
	const std::string key = "SE2_MR_EDGE";

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	auto inserted = registry_.insert({ key, buildEdge() });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getRectangularPrism() {
	const std::string key = "SE2_MR_RPRISM";

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	auto inserted = registry_.insert({ key, buildRectangularPrism() });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getPlane(int subdivisions) {
	if (subdivisions < minSubdivisions) {
		subdivisions = minSubdivisions;
	}

	const std::string key = makePlaneKey(subdivisions);

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	auto inserted = registry_.insert({ key, buildPlane(subdivisions) });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getCone(int slices) {
	if (slices < minSlices) {
		slices = minSlices;
	}

	const std::string key = makeConeKey(slices);

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	auto inserted = registry_.insert({ key, buildCone(slices) });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getCylinder(int slices) {
	if (slices < minSlices) {
		slices = minSlices;
	}

	const std::string key = makeCylinderKey(slices);

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	auto inserted = registry_.insert({ key, buildCylinder(slices) });
	return &inserted.first->second;
}


const EngineUtil::Mesh* MeshRegistry::getEllipsoid(int slices, int stacks) {
	if (slices < minSlices) {
		slices = minSlices;
	}
	if (stacks < minStacks) {
		stacks = minStacks;
	}

	const std::string key = makeEllipsoidKey(slices, stacks);

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	auto inserted = registry_.insert({ key, buildEllipsoid(slices, stacks) });
	return &inserted.first->second;
}

const EngineUtil::Mesh* MeshRegistry::getDotObj(const std::string& filepath) {
	if (filepath.empty()) {
		throw std::invalid_argument("MeshRegistry::getDotObj: filepath cannot be empty");
	}

	const std::string key = makeObjKey(filepath);

	auto found = registry_.find(key);
	if (found != registry_.end()) {
		return &found->second;
	}

	auto inserted = registry_.insert({ key, buildDotObj(filepath) });
	return &inserted.first->second;
}

const EngineRender::GPUMesh* MeshRegistry::getGPUMesh(const std::string& key) {
	auto found = GPURegistry_.find(key);
	if (found == GPURegistry_.end()) {
		return nullptr;
	}
	return found->second.get();
}

const EngineRender::GPUMesh* MeshRegistry::getOrCreateGPUMesh(const std::string& key, const EngineUtil::Mesh& cpuMesh) {
	auto found = GPURegistry_.find(key);
	if (found != GPURegistry_.end()) {
		return found->second.get();
	}

	auto gpuMesh = std::make_unique<EngineRender::GPUMesh>();
	EngineRender::upload(cpuMesh, *gpuMesh);

	EngineRender::GPUMesh* rawPtr = gpuMesh.get();
	GPURegistry_.emplace(key, std::move(gpuMesh));
	return rawPtr;
}

const EngineRender::GPUMesh* MeshRegistry::getGPUEdge() {
	const std::string key = "SE2_MR_EDGE";
	return getOrCreateGPUMesh(key, *getEdge());
}

const EngineRender::GPUMesh* MeshRegistry::getGPURectangularPrism() {
	const std::string key = "SE2_MR_RPRISM";
	return getOrCreateGPUMesh(key, *getRectangularPrism());
}

const EngineRender::GPUMesh* MeshRegistry::getGPUPlane(int subdivisions) {
	if (subdivisions < minSubdivisions) {
		subdivisions = minSubdivisions;
	}
	const std::string key = makePlaneKey(subdivisions);
	return getOrCreateGPUMesh(key, *getPlane(subdivisions));
}

const EngineRender::GPUMesh* MeshRegistry::getGPUCone(int slices) {
	if (slices < minSlices) {
		slices = minSlices;
	}
	const std::string key = makeConeKey(slices);
	return getOrCreateGPUMesh(key, *getCone(slices));
}

const EngineRender::GPUMesh* MeshRegistry::getGPUCylinder(int slices) {
	if (slices < minSlices) {
		slices = minSlices;
	}
	const std::string key = makeCylinderKey(slices);
	return getOrCreateGPUMesh(key, *getCylinder(slices));
}

const EngineRender::GPUMesh* MeshRegistry::getGPUEllipsoid(int slices, int stacks) {
	if (slices < minSlices) {
		slices = minSlices;
	}
	if (stacks < minStacks) {
		stacks = minStacks;
	}
	const std::string key = makeEllipsoidKey(slices, stacks);
	return getOrCreateGPUMesh(key, *getEllipsoid(slices, stacks));
}

const EngineRender::GPUMesh* MeshRegistry::getGPUDotObj(const std::string& filepath) {
	if (filepath.empty()) {
		throw std::invalid_argument("MeshRegistry::getGPUDotObj: filepath cannot be empty");
	}
	const std::string key = makeObjKey(filepath);
	return getOrCreateGPUMesh(key, *getDotObj(filepath));
}

void MeshRegistry::clearRegistry() {
	for (auto& kv : GPURegistry_) {
		kv.second->release();
	}
	GPURegistry_.clear();
	registry_.clear();
}