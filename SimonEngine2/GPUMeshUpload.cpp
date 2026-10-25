#include "GPUMeshUpload.hpp"

void GPUMeshUpload::upload(const EngineUtil::Mesh& CPUMesh, GPUMesh& outGPUMesh) {
	outGPUMesh.release();

	if (CPUMesh.vertices_.empty()) {
		throw std::runtime_error("GPUMeshUpload::upload: mesh has no vertices");
	}

	if (CPUMesh.faceIndices_.empty() && CPUMesh.edgeIndices_.empty()) {
		throw std::runtime_error("GpuMeshUpload::upload: mesh has no face or edge indices");
	}

	glGenVertexArrays(1, &outGPUMesh.vao_);
	glBindVertexArray(outGPUMesh.vao_);

	// Vertex buffer
	glGenBuffers(1, &outGPUMesh.vertexBuffer_);
	glBindBuffer(GL_ARRAY_BUFFER, outGPUMesh.vertexBuffer_);
	glBufferData(
		GL_ARRAY_BUFFER,
		static_cast<GLsizeiptr>(CPUMesh.vertices_.size() * sizeof(EngineUtil::Vertex)),
		CPUMesh.vertices_.data(),
		GL_STATIC_DRAW
	);

	// Attribute 0 = vec3 position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(EngineUtil::Vertex),
		reinterpret_cast<void*>(0)
	);

	// Face index buffer
	if (!CPUMesh.faceIndices_.empty()) {
		glGenBuffers(1, &outGPUMesh.faceIndexBuffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outGPUMesh.faceIndexBuffer_);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			static_cast<GLsizeiptr>(CPUMesh.faceIndices_.size() * sizeof(unsigned int)),
			CPUMesh.faceIndices_.data(),
			GL_STATIC_DRAW
		);

		outGPUMesh.faceIndexCount_ = static_cast<GLsizei>(CPUMesh.faceIndices_.size());
	}
	else {
		outGPUMesh.faceIndexCount_ = 0;
	}

	// Edge index buffer
	if (!CPUMesh.edgeIndices_.empty()) {
		glGenBuffers(1, &outGPUMesh.edgeIndexBuffer_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, outGPUMesh.edgeIndexBuffer_);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			static_cast<GLsizeiptr>(CPUMesh.edgeIndices_.size() * sizeof(unsigned int)),
			CPUMesh.edgeIndices_.data(),
			GL_STATIC_DRAW
		);

		outGPUMesh.edgeIndexCount_ = static_cast<GLsizei>(CPUMesh.edgeIndices_.size());
	}
	else {
		outGPUMesh.edgeIndexCount_ = 0;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}