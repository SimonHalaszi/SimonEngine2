#include "Renderer.hpp"

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

void Renderer::drawMesh(const GPUMesh& mesh, const EngineUtil::Material& material, const EngineMath::Matrix4& model, const EngineMath::Matrix4& viewProjection) const {
	if (mesh.vao_ == 0 || mesh.faceIndexBuffer_ == 0 || mesh.faceIndexCount_ == 0) {
		return;
	}

	const auto found = programRegistry_.find(material.shader_);
	if (found == programRegistry_.end()) {
		throw std::runtime_error("Renderer::drawMesh: unkown shader: " + material.shader_);
	}

	const ShaderProgram& program = found->second;

	glUseProgram(program.id_);

	glUniformMatrix4fv(program.viewProjLocation_, 1, GL_FALSE, viewProjection.data());

	glUniformMatrix4fv(program.modelLocation_, 1, GL_FALSE, model.data());

	if (program.colorLocation_ != -1) {
		glUniform4f(program.colorLocation_, material.color_.r_, material.color_.g_, material.color_.b_, 1.0f);
	}

	glBindVertexArray(mesh.vao_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.faceIndexBuffer_);

	glDrawElements(GL_TRIANGLES, mesh.faceIndexCount_, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
	glUseProgram(0);
}

std::string Renderer::getOrLoadShader(const std::string& vertPath, const std::string& fragPath) {
	const std::string key =
		std::to_string(vertPath.size()) + ":" + vertPath + std::to_string(fragPath.size()) + ":" + fragPath;

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
}

