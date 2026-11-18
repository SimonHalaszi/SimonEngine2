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
        program.textureLocation_ = glGetUniformLocation(program.id_, "uTexture");
        program.uvScaleLocation_ = glGetUniformLocation(program.id_, "uUvScale");
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

	glBindSampler(0, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

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

	if (mode == DrawMode::Solid) {
		indexBuffer = mesh.faceIndexBuffer_;
		indexCount = mesh.faceIndexCount_;
		topology = GL_TRIANGLES;
	}
	if (mode == DrawMode::Edges) {
		indexBuffer = mesh.edgeIndexBuffer_;
		indexCount = mesh.edgeIndexCount_;
		topology = GL_LINES;
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

	if (program.uvScaleLocation_ != -1) {
		glUniform2f(program.uvScaleLocation_, material.uvScaleU_, material.uvScaleV_);
	}

	if (program.textureLocation_ != -1) {
		glUniform1i(program.textureLocation_, 0);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material.albedoTexture_);

	GLuint sampler = 0;
	if (material.albedoTexture_ != 0) {
		sampler = getOrCreateSampler(material.wrapS_, material.wrapT_);
	}
	glBindSampler(0, sampler);

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
	const std::string key = vertPath + fragPath;

	if (programRegistry_.find(key) == programRegistry_.end()) {
		loadShader(key, vertPath, fragPath);
	}

	return key;
}

GLuint Renderer::getOrCreateSampler(GLenum wrapS, GLenum wrapT) const {
	auto validWrap = [](GLenum wrap) {
		return wrap == GL_REPEAT || wrap == GL_MIRRORED_REPEAT || wrap == GL_CLAMP_TO_EDGE;
		};

	if (!validWrap(wrapS) || !validWrap(wrapT)) {
		throw std::invalid_argument("Renderer::getOrCreateSampler: unsupported wrap mode");
	}

	const auto key = std::make_pair(wrapS, wrapT);
	auto it = samplerRegistry_.find(key);
	if (it != samplerRegistry_.end()) {
		return it->second;
	}

	GLuint sampler = 0;
	glGenSamplers(1, &sampler);

	if (sampler == 0) {
		throw std::runtime_error("Renderer::getOrCreateSampler: failed to create sampler");
	}

	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrapS);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrapT);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	try {
		samplerRegistry_.emplace(key, sampler);
	}
	catch (...) {
		glDeleteSamplers(1, &sampler);
		throw;
	}

	return sampler;
}

void Renderer::clear() {
	glUseProgram(0);
	glBindSampler(0, 0);

	for (const auto& entry : programRegistry_) {
		glDeleteProgram(entry.second.id_);
	}
	programRegistry_.clear();

	for (const auto& entry : samplerRegistry_) {
		glDeleteSamplers(1, &entry.second);
	}
	samplerRegistry_.clear();

	resetBindingCache();
	passActive_ = false;
}