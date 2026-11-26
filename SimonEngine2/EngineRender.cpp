#include "EngineRender.hpp"

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

#include "EngineMath.hpp"
#include "EngineUtil.hpp"
#include "Renderer.hpp"

#include "TextureRegistry.hpp"

namespace {
	EngineMath::Vector3 getPosition(const EngineUtil::Vertex& vertex) {
		return { vertex.x_, vertex.y_, vertex.z_ };
	}

	EngineMath::Vector3 getNormal(const EngineUtil::Vertex& vertex) {
		return { vertex.nx_, vertex.ny_, vertex.nz_ };
	}

	void calculateTangents(std::vector<EngineUtil::Vertex>& vertices, const std::vector<unsigned int>& indices) {
		std::vector<EngineMath::Vector3> tangents(vertices.size());
		std::vector<EngineMath::Vector3> bitangents(vertices.size());

		for (std::size_t i = 0; i + 2 < indices.size(); i += 3) {
			EngineUtil::Vertex& first = vertices[indices[i]];
			EngineUtil::Vertex& second = vertices[indices[i + 1]];
			EngineUtil::Vertex& third = vertices[indices[i + 2]];

			const EngineMath::Vector3 edge1 = getPosition(second) - getPosition(first);
			const EngineMath::Vector3 edge2 = getPosition(third) - getPosition(first);
			const float deltaU1 = second.u_ - first.u_;
			const float deltaV1 = second.v_ - first.v_;
			const float deltaU2 = third.u_ - first.u_;
			const float deltaV2 = third.v_ - first.v_;
			const float determinant = deltaU1 * deltaV2 - deltaV1 * deltaU2;

			if (std::abs(determinant) <= 1e-8f) {
				continue;
			}

			const float reciprocalDeterminant = 1.0f / determinant;
			const EngineMath::Vector3 tangent = (edge1 * deltaV2 - edge2 * deltaV1) * reciprocalDeterminant;
			const EngineMath::Vector3 bitangent = (edge2 * deltaU1 - edge1 * deltaU2) * reciprocalDeterminant;

			for (unsigned int index : { indices[i], indices[i + 1], indices[i + 2] }) {
				tangents[index] += tangent;
				bitangents[index] += bitangent;
			}
		}

		for (std::size_t i = 0; i < vertices.size(); ++i) {
			EngineUtil::Vertex& vertex = vertices[i];
			const EngineMath::Vector3 normal = getNormal(vertex).normalized();
			EngineMath::Vector3 tangent = tangents[i] - normal * normal.dot(tangents[i]);

			if (tangent.lengthSquared() <= 1e-16f) {
				const EngineMath::Vector3 axis = std::abs(normal.x_) < 0.9f
					? EngineMath::Vector3(1.0f, 0.0f, 0.0f)
					: EngineMath::Vector3(0.0f, 1.0f, 0.0f);
				tangent = axis.cross(normal);
			}

			tangent = tangent.normalized();
			const float handedness = normal.cross(tangent).dot(bitangents[i]) < 0.0f ? -1.0f : 1.0f;
			vertex.tx_ = tangent.x_;
			vertex.ty_ = tangent.y_;
			vertex.tz_ = tangent.z_;
			vertex.tw_ = handedness;
		}
	}

	void validateUploadMesh(const EngineUtil::Mesh& mesh) {
		if (mesh.vertices_.empty()) {
			throw std::runtime_error("GPUMeshUpload::upload: mesh has no vertices");
		}

		if (mesh.faceIndices_.empty() && mesh.edgeIndices_.empty()) {
			throw std::runtime_error("GPUMeshUpload::upload: mesh has no face or edge indices");
		}

		if ((mesh.faceIndices_.size() % 3u) != 0u) {
			throw std::runtime_error("GPUMeshUpload::upload: face index count must be divisible by 3");
		}

		if ((mesh.edgeIndices_.size() % 2u) != 0u) {
			throw std::runtime_error("GPUMeshUpload::upload: edge index count must be divisible by 2");
		}

		const std::size_t vertexCount = mesh.vertices_.size();

		for (unsigned int index : mesh.faceIndices_) {
			if (static_cast<std::size_t>(index) >= vertexCount) {
				throw std::runtime_error("GPUMeshUpload::upload: face index out of range");
			}
		}

		for (unsigned int index : mesh.edgeIndices_) {
			if (static_cast<std::size_t>(index) >= vertexCount) {
				throw std::runtime_error("GPUMeshUpload::upload: edge index out of range");
			}
		}

		if (mesh.faceIndices_.size() >
			static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
			throw std::runtime_error("GPUMeshUpload::upload: face index count exceeds GLsizei range");
		}

		if (mesh.edgeIndices_.size() >
			static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
			throw std::runtime_error("GPUMeshUpload::upload: edge index count exceeds GLsizei range");
		}

		constexpr std::size_t maxGLSize = static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max());

		const std::size_t vertexBytes = mesh.vertices_.size() * sizeof(EngineUtil::Vertex);
		if (mesh.vertices_.size() != 0 &&
			vertexBytes / sizeof(EngineUtil::Vertex) != mesh.vertices_.size()) {
			throw std::runtime_error("GPUMeshUpload::upload: vertex buffer size overflow");
		}
		if (vertexBytes > maxGLSize) {
			throw std::runtime_error("GPUMeshUpload::upload: vertex buffer exceeds GLsizeiptr range");
		}

		const std::size_t faceBytes = mesh.faceIndices_.size() * sizeof(unsigned int);
		if (mesh.faceIndices_.size() != 0 &&
			faceBytes / sizeof(unsigned int) != mesh.faceIndices_.size()) {
			throw std::runtime_error("GPUMeshUpload::upload: face index buffer size overflow");
		}
		if (faceBytes > maxGLSize) {
			throw std::runtime_error("GPUMeshUpload::upload: face index buffer exceeds GLsizeiptr range");
		}

		const std::size_t edgeBytes = mesh.edgeIndices_.size() * sizeof(unsigned int);
		if (mesh.edgeIndices_.size() != 0 &&
			edgeBytes / sizeof(unsigned int) != mesh.edgeIndices_.size()) {
			throw std::runtime_error("GPUMeshUpload::upload: edge index buffer size overflow");
		}
		if (edgeBytes > maxGLSize) {
			throw std::runtime_error("GPUMeshUpload::upload: edge index buffer exceeds GLsizeiptr range");
		}
	}
}

void EngineRender::upload(const EngineUtil::Mesh& CPUMesh, GPUMesh& outGPUMesh) {
	validateUploadMesh(CPUMesh);
	outGPUMesh.release();
	std::vector<EngineUtil::Vertex> vertices = CPUMesh.vertices_;
	calculateTangents(vertices, CPUMesh.faceIndices_);

	glGenVertexArrays(1, &outGPUMesh.vao_);
	glBindVertexArray(outGPUMesh.vao_);

	// Vertex buffer
	glGenBuffers(1, &outGPUMesh.vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, outGPUMesh.vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(EngineUtil::Vertex)), vertices.data(), GL_STATIC_DRAW);

	// Attribute 0 = vec3 position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EngineUtil::Vertex), reinterpret_cast<void*>(0));

	// Attribute 1 = vec2 texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(EngineUtil::Vertex), reinterpret_cast<void*>(offsetof(EngineUtil::Vertex, u_)));

	// Attribute 2 = vec3 normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(EngineUtil::Vertex), reinterpret_cast<void*>(offsetof(EngineUtil::Vertex, nx_)));

	// Attribute 3 = vec4 tangent and handedness
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(EngineUtil::Vertex), reinterpret_cast<void*>(offsetof(EngineUtil::Vertex, tx_)));

	// Face index buffer
	if (!CPUMesh.faceIndices_.empty()) {
		glGenBuffers(1, &outGPUMesh.faceIndexBuffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outGPUMesh.faceIndexBuffer_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(CPUMesh.faceIndices_.size() * sizeof(unsigned int)), CPUMesh.faceIndices_.data(), GL_STATIC_DRAW);

		outGPUMesh.faceIndexCount_ = static_cast<GLsizei>(CPUMesh.faceIndices_.size());
	}
	else {
		outGPUMesh.faceIndexCount_ = 0;
	}

	// Edge index buffer
	if (!CPUMesh.edgeIndices_.empty()) {
		glGenBuffers(1, &outGPUMesh.edgeIndexBuffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outGPUMesh.edgeIndexBuffer_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(CPUMesh.edgeIndices_.size() * sizeof(unsigned int)), CPUMesh.edgeIndices_.data(), GL_STATIC_DRAW);

		outGPUMesh.edgeIndexCount_ = static_cast<GLsizei>(CPUMesh.edgeIndices_.size());
	}
	else {
		outGPUMesh.edgeIndexCount_ = 0;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EngineRender::Material::init() {
	shader_ = Renderer::getInstance().getOrLoadShader(vertPath_, fragPath_);

	albedoTexture_ = 0;
	if (!albedoTexturePath_.empty()) {
		albedoTexture_ = TextureRegistry::getInstance().loadTexture(albedoTexturePath_);
	}

	normalTexture_ = 0;
	if (!normalTexturePath_.empty()) {
		normalTexture_ = TextureRegistry::getInstance().loadTexture(normalTexturePath_);
	}

	roughnessTexture_ = 0;
	if (!roughnessTexturePath_.empty()) {
		roughnessTexture_ = TextureRegistry::getInstance().loadTexture(roughnessTexturePath_);
	}

	metallicTexture_ = 0;
	if (!metallicTexturePath_.empty()) {
		metallicTexture_ = TextureRegistry::getInstance().loadTexture(metallicTexturePath_);
	}
}