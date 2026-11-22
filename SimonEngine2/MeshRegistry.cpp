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

		auto addFace = [&mesh](unsigned int a, unsigned int b, unsigned int c, unsigned int d, const EngineMath::Vector3& normal) {
			const EngineUtil::Vertex corners[] = {
				mesh.vertices_[a], mesh.vertices_[b], mesh.vertices_[c], mesh.vertices_[d]
			};
			const float uv[4][2] = {
				{ 0.0f, 0.0f }, { 1.0f, 0.0f },
				{ 1.0f, 1.0f }, { 0.0f, 1.0f }
			};
			const unsigned int base = static_cast<unsigned int>(mesh.vertices_.size());

			for (unsigned int i = 0; i < 4; ++i) {
				mesh.vertices_.push_back({
					corners[i].x_, corners[i].y_, corners[i].z_,
					uv[i][0], uv[i][1],
					normal.x_, normal.y_, normal.z_
					});
			}

			mesh.faceIndices_.insert(mesh.faceIndices_.end(), {
				base, base + 1, base + 2, base, base + 2, base + 3
				});
			};

		addFace(0u, 1u, 2u, 3u, { 0.0f, 0.0f, 1.0f });
		addFace(5u, 4u, 7u, 6u, { 0.0f, 0.0f, -1.0f });
		addFace(4u, 0u, 3u, 7u, { -1.0f, 0.0f, 0.0f });
		addFace(1u, 5u, 6u, 2u, { 1.0f, 0.0f, 0.0f });
		addFace(3u, 2u, 6u, 7u, { 0.0f, 1.0f, 0.0f });
		addFace(4u, 5u, 1u, 0u, { 0.0f, -1.0f, 0.0f });

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
				const float u = static_cast<float>(x) / static_cast<float>(grid);
				const float v = static_cast<float>(z) / static_cast<float>(grid);

				mesh.vertices_.push_back({ vx, 0.0f, vz, u, v, 0.0f, 1.0f, 0.0f });
			}
		}

		auto idx = [verticesPerSide](int x, int z) -> unsigned int {
			return static_cast<unsigned int>(z * verticesPerSide + x);
			};

		for (int z = 0; z < grid; ++z) {
			for (int x = 0; x < grid; ++x) {
				const unsigned int a = idx(x, z);
				const unsigned int b = idx(x + 1, z);
				const unsigned int c = idx(x + 1, z + 1);
				const unsigned int d = idx(x, z + 1);

				mesh.faceIndices_.insert(mesh.faceIndices_.end(), { a, c, b, a, d, c });
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
			mesh.vertices_.push_back({ x, -halfUnit, z, 0.0f, 0.0f });
		}

		const unsigned int tip = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, halfUnit, 0.0f, 0.0f, 0.0f });

		const unsigned int baseCenter = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, -halfUnit, 0.0f, 0.0f, 0.0f });

		auto addFaceTriangle = [&mesh](unsigned int a, float au, float av, const EngineMath::Vector3& an, unsigned int b, float bu, float bv, const EngineMath::Vector3& bn, unsigned int c, float cu, float cv, const EngineMath::Vector3& cn) {
			const EngineUtil::Vertex va = mesh.vertices_[a];
			const EngineUtil::Vertex vb = mesh.vertices_[b];
			const EngineUtil::Vertex vc = mesh.vertices_[c];
			const unsigned int first = static_cast<unsigned int>(mesh.vertices_.size());

			mesh.vertices_.push_back({ va.x_, va.y_, va.z_, au, av, an.x_, an.y_, an.z_ });
			mesh.vertices_.push_back({ vb.x_, vb.y_, vb.z_, bu, bv, bn.x_, bn.y_, bn.z_ });
			mesh.vertices_.push_back({ vc.x_, vc.y_, vc.z_, cu, cv, cn.x_, cn.y_, cn.z_ });
			mesh.faceIndices_.insert(mesh.faceIndices_.end(), { first, first + 1, first + 2 });
			};

		for (int slice = 0; slice < slices; ++slice) {
			const unsigned int current = static_cast<unsigned int>(slice);
			const unsigned int next = static_cast<unsigned int>((slice + 1) % slices);
			const float u0 = static_cast<float>(slice) / static_cast<float>(slices);
			const float u1 = static_cast<float>(slice + 1) / static_cast<float>(slices);

			const EngineUtil::Vertex center = mesh.vertices_[baseCenter];
			const EngineUtil::Vertex ring0 = mesh.vertices_[current];
			const EngineUtil::Vertex ring1 = mesh.vertices_[next];

			const EngineMath::Vector3 normal0 = EngineMath::Vector3(ring0.x_, halfUnit * halfUnit, ring0.z_).normalized();
			const EngineMath::Vector3 normal1 = EngineMath::Vector3(ring1.x_, halfUnit * halfUnit, ring1.z_).normalized();

			const float midTheta = 2.0f * EngineMath::PI * (u0 + u1) * 0.5f;
			const EngineMath::Vector3 tipNormal = EngineMath::Vector3(halfUnit * std::cos(midTheta), halfUnit * halfUnit, halfUnit * std::sin(midTheta)).normalized();

			addFaceTriangle(current, u0, 0.0f, normal0, tip, (u0 + u1) * 0.5f, 1.0f, tipNormal, next, u1, 0.0f, normal1);

			const EngineMath::Vector3 baseNormal(0.0f, -1.0f, 0.0f);
			addFaceTriangle(
				baseCenter, center.x_ + 0.5f, center.z_ + 0.5f, baseNormal,
				current, ring0.x_ + 0.5f, ring0.z_ + 0.5f, baseNormal,
				next, ring1.x_ + 0.5f, ring1.z_ + 0.5f, baseNormal
			);

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

		for (int slice = 0; slice < slices; ++slice) {
			const float theta = 2.0f * EngineMath::PI * static_cast<float>(slice) / static_cast<float>(slices);
			mesh.vertices_.push_back({ halfUnit * std::cos(theta), -halfUnit, halfUnit * std::sin(theta), 0.0f, 0.0f });
		}

		for (int slice = 0; slice < slices; ++slice) {
			const float theta = 2.0f * EngineMath::PI * static_cast<float>(slice) / static_cast<float>(slices);
			mesh.vertices_.push_back({ halfUnit * std::cos(theta), halfUnit, halfUnit * std::sin(theta), 0.0f, 0.0f });
		}

		const unsigned int bottomCenter = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, -halfUnit, 0.0f, 0.0f, 0.0f });

		const unsigned int topCenter = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, halfUnit, 0.0f, 0.0f, 0.0f });

		auto addFaceTriangle = [&mesh](unsigned int a, float au, float av, const EngineMath::Vector3& an, unsigned int b, float bu, float bv, const EngineMath::Vector3& bn, unsigned int c, float cu, float cv, const EngineMath::Vector3& cn) {
			const EngineUtil::Vertex va = mesh.vertices_[a];
			const EngineUtil::Vertex vb = mesh.vertices_[b];
			const EngineUtil::Vertex vc = mesh.vertices_[c];
			const unsigned int first = static_cast<unsigned int>(mesh.vertices_.size());

			mesh.vertices_.push_back({ va.x_, va.y_, va.z_, au, av, an.x_, an.y_, an.z_ });
			mesh.vertices_.push_back({ vb.x_, vb.y_, vb.z_, bu, bv, bn.x_, bn.y_, bn.z_ });
			mesh.vertices_.push_back({ vc.x_, vc.y_, vc.z_, cu, cv, cn.x_, cn.y_, cn.z_ });
			mesh.faceIndices_.insert(mesh.faceIndices_.end(), { first, first + 1, first + 2 });
			};

		for (int slice = 0; slice < slices; ++slice) {
			const unsigned int next = static_cast<unsigned int>((slice + 1) % slices);
			const unsigned int b0 = static_cast<unsigned int>(slice);
			const unsigned int b1 = next;
			const unsigned int t0 = static_cast<unsigned int>(slice + slices);
			const unsigned int t1 = static_cast<unsigned int>(next + slices);
			const float u0 = static_cast<float>(slice) / static_cast<float>(slices);
			const float u1 = static_cast<float>(slice + 1) / static_cast<float>(slices);

			const EngineUtil::Vertex ring0 = mesh.vertices_[b0];
			const EngineUtil::Vertex ring1 = mesh.vertices_[b1];
			const EngineMath::Vector3 normal0 = EngineMath::Vector3(ring0.x_, 0.0f, ring0.z_).normalized();
			const EngineMath::Vector3 normal1 = EngineMath::Vector3(ring1.x_, 0.0f, ring1.z_).normalized();

			addFaceTriangle(b0, u0, 0.0f, normal0, t0, u0, 1.0f, normal0, b1, u1, 0.0f, normal1);
			addFaceTriangle(b1, u1, 0.0f, normal1, t0, u0, 1.0f, normal0, t1, u1, 1.0f, normal1);

			auto capU = [](const EngineUtil::Vertex& vertex) { return vertex.x_ + 0.5f; };
			auto capV = [](const EngineUtil::Vertex& vertex) { return vertex.z_ + 0.5f; };
			const EngineMath::Vector3 bottomNormal(0.0f, -1.0f, 0.0f);
			const EngineMath::Vector3 topNormal(0.0f, 1.0f, 0.0f);

			addFaceTriangle(
				bottomCenter, capU(mesh.vertices_[bottomCenter]), capV(mesh.vertices_[bottomCenter]), bottomNormal,
				b0, capU(mesh.vertices_[b0]), capV(mesh.vertices_[b0]), bottomNormal,
				b1, capU(mesh.vertices_[b1]), capV(mesh.vertices_[b1]), bottomNormal
			);
			addFaceTriangle(
				topCenter, capU(mesh.vertices_[topCenter]), capV(mesh.vertices_[topCenter]), topNormal,
				t1, capU(mesh.vertices_[t1]), capV(mesh.vertices_[t1]), topNormal,
				t0, capU(mesh.vertices_[t0]), capV(mesh.vertices_[t0]), topNormal
			);

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
		mesh.vertices_.push_back({ 0.0f, -halfUnit, 0.0f, 0.0f, 0.0f });

		const int ringCount = stacks - 1;
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
				mesh.vertices_.push_back({ x, y, z, 0.0f, 0.0f });
			}
		}

		const unsigned int northPole = static_cast<unsigned int>(mesh.vertices_.size());
		mesh.vertices_.push_back({ 0.0f, halfUnit, 0.0f, 0.0f, 0.0f });

		auto ringVertex = [vertsPerRing](int ringIndex, int sliceIndex) -> unsigned int {
			const int wrappedSlice = (sliceIndex % vertsPerRing + vertsPerRing) % vertsPerRing;
			return static_cast<unsigned int>(1 + ringIndex * vertsPerRing + wrappedSlice);
			};

		auto addFaceTriangle = [&mesh](unsigned int a, float au, float av, unsigned int b, float bu, float bv, unsigned int c, float cu, float cv) {
			const EngineUtil::Vertex va = mesh.vertices_[a];
			const EngineUtil::Vertex vb = mesh.vertices_[b];
			const EngineUtil::Vertex vc = mesh.vertices_[c];
			const unsigned int first = static_cast<unsigned int>(mesh.vertices_.size());

			const EngineMath::Vector3 na = EngineMath::Vector3(va.x_, va.y_, va.z_).normalized();
			const EngineMath::Vector3 nb = EngineMath::Vector3(vb.x_, vb.y_, vb.z_).normalized();
			const EngineMath::Vector3 nc = EngineMath::Vector3(vc.x_, vc.y_, vc.z_).normalized();

			mesh.vertices_.push_back({ va.x_, va.y_, va.z_, au, av, na.x_, na.y_, na.z_ });
			mesh.vertices_.push_back({ vb.x_, vb.y_, vb.z_, bu, bv, nb.x_, nb.y_, nb.z_ });
			mesh.vertices_.push_back({ vc.x_, vc.y_, vc.z_, cu, cv, nc.x_, nc.y_, nc.z_ });
			mesh.faceIndices_.insert(mesh.faceIndices_.end(), { first, first + 1, first + 2 });
			};

		const int lastRing = ringCount - 1;

		for (int slice = 0; slice < slices; ++slice) {
			const unsigned int a = ringVertex(0, slice);
			const unsigned int b = ringVertex(0, slice + 1);
			const float u0 = static_cast<float>(slice) / static_cast<float>(slices);
			const float u1 = static_cast<float>(slice + 1) / static_cast<float>(slices);
			const float v = 1.0f - 1.0f / static_cast<float>(stacks);

			addFaceTriangle(northPole, (u0 + u1) * 0.5f, 1.0f, b, u1, v, a, u0, v);
		}

		for (int ring = 0; ring < lastRing; ++ring) {
			for (int slice = 0; slice < slices; ++slice) {
				const unsigned int a = ringVertex(ring, slice);
				const unsigned int b = ringVertex(ring, slice + 1);
				const unsigned int c = ringVertex(ring + 1, slice + 1);
				const unsigned int d = ringVertex(ring + 1, slice);
				const float u0 = static_cast<float>(slice) / static_cast<float>(slices);
				const float u1 = static_cast<float>(slice + 1) / static_cast<float>(slices);
				const float v0 = 1.0f - static_cast<float>(ring + 1) / static_cast<float>(stacks);
				const float v1 = 1.0f - static_cast<float>(ring + 2) / static_cast<float>(stacks);

				addFaceTriangle(a, u0, v0, b, u1, v0, d, u0, v1);
				addFaceTriangle(b, u1, v0, c, u1, v1, d, u0, v1);
			}
		}

		for (int slice = 0; slice < slices; ++slice) {
			const unsigned int a = ringVertex(lastRing, slice);
			const unsigned int b = ringVertex(lastRing, slice + 1);
			const float u0 = static_cast<float>(slice) / static_cast<float>(slices);
			const float u1 = static_cast<float>(slice + 1) / static_cast<float>(slices);
			const float v = 1.0f / static_cast<float>(stacks);

			addFaceTriangle(southPole, (u0 + u1) * 0.5f, 0.0f, a, u0, v, b, u1, v);
		}

		for (int slice = 0; slice < slices; ++slice) {
			mesh.edgeIndices_.insert(mesh.edgeIndices_.end(), { northPole, ringVertex(0, slice) });

			for (int ring = 0; ring < lastRing; ++ring) {
				mesh.edgeIndices_.insert(mesh.edgeIndices_.end(), { ringVertex(ring, slice), ringVertex(ring + 1, slice) });
			}

			mesh.edgeIndices_.insert(mesh.edgeIndices_.end(), { ringVertex(lastRing, slice), southPole });
		}

		for (int ring = 0; ring < ringCount; ++ring) {
			for (int slice = 0; slice < slices; ++slice) {
				mesh.edgeIndices_.insert(mesh.edgeIndices_.end(), { ringVertex(ring, slice), ringVertex(ring, slice + 1) });
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

	struct ObjFaceVertex {
		int positionIndex_;
		int textureIndex_;
		int normalIndex_;
	};

	struct ObjPoint2D {
		double x_;
		double y_;
	};

	double cross2D(const ObjPoint2D& a, const ObjPoint2D& b, const ObjPoint2D& c) {
		return (b.x_ - a.x_) * (c.y_ - a.y_) - (b.y_ - a.y_) * (c.x_ - a.x_);
	}

	bool pointInTriangle2D(const ObjPoint2D& a, const ObjPoint2D& b, const ObjPoint2D& c, const ObjPoint2D& point, double winding, double epsilon) {
		return cross2D(a, b, point) * winding >= -epsilon &&
			cross2D(b, c, point) * winding >= -epsilon &&
			cross2D(c, a, point) * winding >= -epsilon;
	}

	std::vector<ObjFaceVertex> triangulateObjPolygon(const std::vector<ObjFaceVertex>& polygon, const std::vector<EngineUtil::Vertex>& positions) {
		double normalX = 0.0;
		double normalY = 0.0;
		double normalZ = 0.0;

		for (std::size_t i = 0; i < polygon.size(); ++i) {
			const EngineUtil::Vertex& current = positions[polygon[i].positionIndex_];
			const EngineUtil::Vertex& next = positions[polygon[(i + 1) % polygon.size()].positionIndex_];

			normalX += static_cast<double>(current.y_ - next.y_) * (current.z_ + next.z_);
			normalY += static_cast<double>(current.z_ - next.z_) * (current.x_ + next.x_);
			normalZ += static_cast<double>(current.x_ - next.x_) * (current.y_ + next.y_);
		}

		const double absNormalX = std::abs(normalX);
		const double absNormalY = std::abs(normalY);
		const double absNormalZ = std::abs(normalZ);
		const int droppedAxis = absNormalX > absNormalY && absNormalX > absNormalZ ? 0 : absNormalY > absNormalZ ? 1 : 2;

		std::vector<ObjPoint2D> projected;
		projected.reserve(polygon.size());

		for (const ObjFaceVertex& corner : polygon) {
			const EngineUtil::Vertex& position = positions[corner.positionIndex_];
			if (droppedAxis == 0) {
				projected.push_back({ position.y_, position.z_ });
			}
			else if (droppedAxis == 1) {
				projected.push_back({ position.x_, position.z_ });
			}
			else {
				projected.push_back({ position.x_, position.y_ });
			}
		}

		double signedArea = 0.0;
		for (std::size_t i = 0; i < projected.size(); ++i) {
			const ObjPoint2D& current = projected[i];
			const ObjPoint2D& next = projected[(i + 1) % projected.size()];
			signedArea += current.x_ * next.y_ - next.x_ * current.y_;
		}

		const double epsilon = std::max(std::abs(signedArea) * 1e-12, 1e-20);
		const double winding = signedArea > 0.0 ? 1.0 : -1.0;
		bool isConcave = false;
		for (std::size_t i = 0; i < projected.size(); ++i) {
			const ObjPoint2D& previous = projected[(i + projected.size() - 1) % projected.size()];
			const ObjPoint2D& current = projected[i];
			const ObjPoint2D& next = projected[(i + 1) % projected.size()];

			if (cross2D(previous, current, next) * winding < -epsilon) {
				isConcave = true;
				break;
			}
		}

		std::vector<ObjFaceVertex> triangles;
		triangles.reserve((polygon.size() - 2) * 3);

		if (!isConcave) {
			for (std::size_t i = 1; i + 1 < polygon.size(); ++i) {
				triangles.push_back(polygon[0]);
				triangles.push_back(polygon[i]);
				triangles.push_back(polygon[i + 1]);
			}
			return triangles;
		}

		std::vector<std::size_t> remaining(polygon.size());
		for (std::size_t i = 0; i < remaining.size(); ++i) {
			remaining[i] = i;
		}

		while (remaining.size() > 3) {
			for (std::size_t i = 0; i < remaining.size(); ++i) {
				const std::size_t previous = remaining[(i + remaining.size() - 1) % remaining.size()];
				const std::size_t current = remaining[i];
				const std::size_t next = remaining[(i + 1) % remaining.size()];

				if (cross2D(projected[previous], projected[current], projected[next]) * winding <= epsilon) {
					continue;
				}

				bool containsVertex = false;
				for (const std::size_t candidate : remaining) {
					if (candidate == previous || candidate == current || candidate == next) {
						continue;
					}

					if (pointInTriangle2D(projected[previous], projected[current], projected[next], projected[candidate], winding, epsilon)) {
						containsVertex = true;
						break;
					}
				}

				if (containsVertex) {
					continue;
				}

				triangles.push_back(polygon[previous]);
				triangles.push_back(polygon[current]);
				triangles.push_back(polygon[next]);
				remaining.erase(remaining.begin() + i);
				break;
			}
		}

		for (const std::size_t index : remaining) {
			triangles.push_back(polygon[index]);
		}
		return triangles;
	}

	EngineUtil::Mesh buildDotObj(const std::string& filepath) {
		std::ifstream file(filepath);

		if (!file) {
			throw std::runtime_error("MeshRegistry::getDotObj: failed to open file: " + filepath);
		}

		struct ObjTextureCoord {
			float u_;
			float v_;
		};

		EngineUtil::Mesh mesh;
		std::vector<EngineUtil::Vertex> positions;
		std::vector<ObjTextureCoord> textureCoords;
		std::vector<EngineMath::Vector3> normals;
		std::vector<ObjFaceVertex> faceCorners;
		std::unordered_set<std::uint64_t> uniqueEdges;

		auto parseIndex = [](const std::string& text, std::size_t count, int& resolvedIndex) {
			if (text.empty()) {
				return false;
			}

			try {
				std::size_t consumed = 0;
				const int objIndex = std::stoi(text, &consumed);
				if (consumed != text.size()) {
					return false;
				}

				resolvedIndex = resolveObjIndex(objIndex, static_cast<int>(count));
				return resolvedIndex >= 0 && static_cast<std::size_t>(resolvedIndex) < count;
			}
			catch (const std::invalid_argument&) {
				return false;
			}
			catch (const std::out_of_range&) {
				return false;
			}
			};

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
					positions.push_back({ x, y, z, 0.0f, 0.0f });
				}

				continue;
			}

			if (type == "vt") {
				float u = 0.0f;
				float v = 0.0f;

				if (stream >> u >> v) {
					textureCoords.push_back({ u, v });
				}

				continue;
			}

			if (type == "vn") {
				float x = 0.0f;
				float y = 0.0f;
				float z = 0.0f;

				if (!(stream >> x >> y >> z)) {
					throw std::runtime_error("MeshRegistry::getDotObj: invalid normal in: " + filepath);
				}

				EngineMath::Vector3 normal(x, y, z);
				if (normal.lengthSquared() <= 1e-16f) {
					throw std::runtime_error("MeshRegistry::getDotObj: zero-length normal in: " + filepath);
				}

				normals.push_back(normal.normalized());
				continue;
			}

			if (type != "f") {
				continue;
			}

			std::vector<ObjFaceVertex> polygon;
			std::string token;
			bool validPolygon = true;

			while (stream >> token) {
				if (!token.empty() && token[0] == '#') {
					break;
				}

				const std::size_t firstSlash = token.find('/');
				const int positionCount = static_cast<int>(positions.size());
				int positionIndex = -1;

				if (!parsePositionIndex(token, positionCount, positionIndex)) {
					validPolygon = false;
					break;
				}

				int textureIndex = -1;
				int normalIndex = -1;

				if (firstSlash != std::string::npos) {
					const std::size_t textureStart = firstSlash + 1;
					const std::size_t secondSlash = token.find('/', textureStart);
					const std::size_t textureEnd = secondSlash == std::string::npos ? token.size() : secondSlash;

					if (textureEnd > textureStart &&
						!parseIndex(token.substr(textureStart, textureEnd - textureStart), textureCoords.size(), textureIndex)) {
						validPolygon = false;
						break;
					}

					if (secondSlash != std::string::npos) {
						const std::string normalPart = token.substr(secondSlash + 1);

						if (!normalPart.empty() && !parseIndex(normalPart, normals.size(), normalIndex)) {
							throw std::runtime_error("MeshRegistry::getDotObj: invalid normal index in: " + filepath);
						}
					}
				}

				polygon.push_back({ positionIndex, textureIndex, normalIndex });
			}

			if (!validPolygon || polygon.size() < 3) {
				continue;
			}

			for (std::size_t i = 0; i < polygon.size(); ++i) {
				const unsigned int first = static_cast<unsigned int>(polygon[i].positionIndex_);
				const unsigned int second = static_cast<unsigned int>(polygon[(i + 1) % polygon.size()].positionIndex_);
				addEdgeUnique(mesh, uniqueEdges, first, second);
			}

			const std::vector<ObjFaceVertex> triangles = triangulateObjPolygon(polygon, positions);
			faceCorners.insert(faceCorners.end(), triangles.begin(), triangles.end());
		}

		if (positions.empty()) {
			throw std::runtime_error("MeshRegistry::getDotObj: file contains no vertices: " + filepath);
		}

		if (faceCorners.empty()) {
			throw std::runtime_error("MeshRegistry::getDotObj: file contains no valid faces: " + filepath);
		}

		mesh.vertices_ = positions;
		mesh.vertices_.reserve(positions.size() + faceCorners.size());

		for (std::size_t i = 0; i < faceCorners.size(); i += 3) {
			const ObjFaceVertex& a = faceCorners[i];
			const ObjFaceVertex& b = faceCorners[i + 1];
			const ObjFaceVertex& c = faceCorners[i + 2];

			EngineMath::Vector3 faceNormal;

			if (a.normalIndex_ < 0 || b.normalIndex_ < 0 || c.normalIndex_ < 0) {
				const EngineUtil::Vertex& pa = positions[a.positionIndex_];
				const EngineUtil::Vertex& pb = positions[b.positionIndex_];
				const EngineUtil::Vertex& pc = positions[c.positionIndex_];

				const EngineMath::Vector3 edge1(pb.x_ - pa.x_, pb.y_ - pa.y_, pb.z_ - pa.z_);
				const EngineMath::Vector3 edge2(pc.x_ - pa.x_, pc.y_ - pa.y_, pc.z_ - pa.z_);
				faceNormal = edge1.cross(edge2);

				if (faceNormal.lengthSquared() <= 1e-16f) {
					throw std::runtime_error("MeshRegistry::getDotObj: degenerate face without normals in: " + filepath);
				}

				faceNormal = faceNormal.normalized();
			}

			for (std::size_t cornerIndex = i; cornerIndex < i + 3; ++cornerIndex) {
				const ObjFaceVertex& corner = faceCorners[cornerIndex];
				const EngineUtil::Vertex& position = positions[corner.positionIndex_];
				const EngineMath::Vector3& normal = corner.normalIndex_ >= 0 ? normals[corner.normalIndex_] : faceNormal;

				float u = 0.0f;
				float v = 0.0f;

				if (corner.textureIndex_ >= 0) {
					u = textureCoords[corner.textureIndex_].u_;
					v = textureCoords[corner.textureIndex_].v_;
				}

				const unsigned int index = static_cast<unsigned int>(mesh.vertices_.size());
				mesh.vertices_.push_back({ position.x_, position.y_, position.z_, u, v, normal.x_, normal.y_, normal.z_ });
				mesh.faceIndices_.push_back(index);
			}
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

void MeshRegistry::clear() {
	for (auto& kv : GPURegistry_) {
		kv.second->release();
	}
	GPURegistry_.clear();
	registry_.clear();
}