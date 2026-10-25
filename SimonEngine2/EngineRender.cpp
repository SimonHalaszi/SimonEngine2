#include "EngineRender.hpp"

namespace {
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

	glGenVertexArrays(1, &outGPUMesh.vao_);
	glBindVertexArray(outGPUMesh.vao_);

	// Vertex buffer
	glGenBuffers(1, &outGPUMesh.vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, outGPUMesh.vertexBuffer_);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(CPUMesh.vertices_.size() * sizeof(EngineUtil::Vertex)), CPUMesh.vertices_.data(), GL_STATIC_DRAW);

	// Attribute 0 = vec3 position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(EngineUtil::Vertex), reinterpret_cast<void*>(0));

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