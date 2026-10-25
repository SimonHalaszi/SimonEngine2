#include "Renderer.hpp"
#include "Camera.hpp"

void Renderer::loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
	if (programRegistry_.find(name) != programRegistry_.end()) {
		throw std::runtime_error("Renderer::loadShader: shader name already registered: " + name);
	}

	ShaderProgram program;
	program.id_ = EngineShader::loadProgram(vertPath, fragPath);

	if (program.id_ == 0) {
		throw std::runtime_error("Renderer::loadShader: failed to load shader: " + name);
	}

	try {
		program.viewProjLocation_ = glGetUniformLocation(program.id_, "uViewProjection");
		program.modelLocation_ = glGetUniformLocation(program.id_, "uModel");
		program.colorLocation_ = glGetUniformLocation(program.id_, "uColor");

		program.timeLocation_ = glGetUniformLocation(program.id_, "uTime");
		program.resolutionLocation_ = glGetUniformLocation(program.id_, "uResolution");

		if (program.viewProjLocation_ == -1 || program.modelLocation_ == -1) {
			throw std::runtime_error("Renderer::loadShader: shader must use uViewProjection and uModel: " + name);
		}

		programRegistry_.emplace(name, program);
	}
	catch (...) {
		glDeleteProgram(program.id_);
		throw;
	}
}

bool Renderer::beginPass(
	const Camera& camera,
	int x, int y,
	int width, int height,
	float time
) {
	if (passActive_) {
		throw std::logic_error("Renderer::beginPass: a pass is already active");
	}

	if (width <= 0 || height <= 0) {
		return false;
	}

	const float aspect = static_cast<float>(width) / static_cast<float>(height);

	viewProjection_ = camera.getViewProjectionMatrix(aspect);

	passTime_ = time;
	passResolutionX_ = static_cast<float>(width);
	passResolutionY_ = static_cast<float>(height);

	glViewport(x, y, width, height);

	resetBindingCache();
	passActive_ = true;
	return true;
}

void Renderer::endPass() {
	glBindVertexArray(0);
	glUseProgram(0);

	resetBindingCache();
	passActive_ = false;
}

void Renderer::drawMesh(const EngineRender::GPUMesh& mesh, const EngineRender::Material& material, const EngineMath::Matrix4& model, DrawMode mode) const {
	if (!passActive_) {
		throw std::logic_error("Renderer::drawMesh: no active render pass");
	}

	GLuint indexBuffer = 0;
	GLsizei indexCount = 0;
	GLenum topology = GL_TRIANGLES;

	switch (mode) {
	case DrawMode::Solid:
		indexBuffer = mesh.faceIndexBuffer_;
		indexCount = mesh.faceIndexCount_;
		topology = GL_TRIANGLES;
		break;
	case DrawMode::Edges:
		indexBuffer = mesh.edgeIndexBuffer_;
		indexCount = mesh.edgeIndexCount_;
		topology = GL_LINES;
		break;
	default:
		throw std::invalid_argument("Renderer::drawMesh: invalid draw mode");
	}

	if (mesh.vao_ == 0 || indexBuffer == 0 || indexCount <= 0) {
		return;
	}

	const auto found = programRegistry_.find(material.shader_);
	if (found == programRegistry_.end()) {
		throw std::runtime_error("Renderer::drawMesh: unknown shader: " + material.shader_);
	}

	const ShaderProgram& program = found->second;

	if (program.id_ != boundProgram_) {
		glUseProgram(program.id_);
		glUniformMatrix4fv(program.viewProjLocation_, 1, GL_FALSE, viewProjection_.data());

		if (program.timeLocation_ != -1) {
			glUniform1f(program.timeLocation_, passTime_);
		}

		if (program.resolutionLocation_ != -1) {
			glUniform2f(program.resolutionLocation_, passResolutionX_, passResolutionY_);
		}


		boundProgram_ = program.id_;
	}

	glUniformMatrix4fv(program.modelLocation_, 1, GL_FALSE, model.data());

	if (program.colorLocation_ != -1) {
		glUniform4f(program.colorLocation_, material.color_.r_, material.color_.g_, material.color_.b_, 1.0f);
	}

	if (mesh.vao_ != boundVAO_) {
		glBindVertexArray(mesh.vao_);
		boundVAO_ = mesh.vao_;
		boundElementBuffer_ = 0;
	}

	if (indexBuffer != boundElementBuffer_) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		boundElementBuffer_ = indexBuffer;
	}

	glDrawElements(topology, indexCount, GL_UNSIGNED_INT, nullptr);
}

std::string Renderer::getOrLoadShader(const std::string& vertPath, const std::string& fragPath) {
	const std::string key = std::to_string(vertPath.size()) + ":" + vertPath + std::to_string(fragPath.size()) + ":" + fragPath;

	if (programRegistry_.find(key) == programRegistry_.end()) {
		loadShader(key, vertPath, fragPath);
	}

	return key;
}

void Renderer::shutdown() {
	glUseProgram(0);

	for (const auto& entry : programRegistry_) {
		glDeleteProgram(entry.second.id_);
	}

	programRegistry_.clear();
	resetBindingCache();
	passActive_ = false;
}