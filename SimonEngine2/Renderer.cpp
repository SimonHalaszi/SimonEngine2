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
		program.normalTextureLocation_ = glGetUniformLocation(program.id_, "uNormalTexture");
		program.roughnessTextureLocation_ = glGetUniformLocation(program.id_, "uRoughnessTexture");
		program.metallicTextureLocation_ = glGetUniformLocation(program.id_, "uMetallicTexture");
		program.timeLocation_ = glGetUniformLocation(program.id_, "uTime");
		program.resolutionLocation_ = glGetUniformLocation(program.id_, "uResolution");
		program.uvScaleLocation_ = glGetUniformLocation(program.id_, "uUvScale");
		program.hasTextureLocation_ = glGetUniformLocation(program.id_, "uHasTexture");
		program.hasNormalTextureLocation_ = glGetUniformLocation(program.id_, "uHasNormalTexture");
		program.hasRoughnessTextureLocation_ = glGetUniformLocation(program.id_, "uHasRoughnessTexture");
		program.hasMetallicTextureLocation_ = glGetUniformLocation(program.id_, "uHasMetallicTexture");
		program.roughnessLocation_ = glGetUniformLocation(program.id_, "uRoughness");
		program.metallicLocation_ = glGetUniformLocation(program.id_, "uMetallic");
		program.viewPositionLocation_ = glGetUniformLocation(program.id_, "uViewPosition");

		program.directionalLightCountLocation_ = glGetUniformLocation(program.id_, "uDirectionalLightCount");
		for (int i = 0; i < maxDirectionalLights_; ++i) {
			const std::string prefix = "uDirectionalLights[" + std::to_string(i) + "].";
			auto& locations = program.directionalLightLocations_[i];

			locations.direction_ = glGetUniformLocation(program.id_, (prefix + "direction").c_str());
			locations.color_ = glGetUniformLocation(program.id_, (prefix + "color").c_str());
			locations.intensity_ = glGetUniformLocation(program.id_, (prefix + "intensity").c_str());
		}

		program.pointLightCountLocation_ = glGetUniformLocation(program.id_, "uPointLightCount");
		if (program.pointLightCountLocation_ != -1) {
			for (int i = 0; i < maxPointLights_; ++i) {
				const std::string prefix = "uPointLights[" + std::to_string(i) + "].";
				auto& locations = program.pointLightLocations_[i];

				locations.position_ = glGetUniformLocation(program.id_, (prefix + "position").c_str());
				locations.color_ = glGetUniformLocation(program.id_, (prefix + "color").c_str());
				locations.intensity_ = glGetUniformLocation(program.id_, (prefix + "intensity").c_str());
				locations.range_ = glGetUniformLocation(program.id_, (prefix + "range").c_str());
			}
		}

		program.spotLightCountLocation_ = glGetUniformLocation(program.id_, "uSpotLightCount");
		if (program.spotLightCountLocation_ != -1) {
			for (int i = 0; i < maxSpotLights_; ++i) {
				const std::string prefix = "uSpotLights[" + std::to_string(i) + "].";
				auto& locations = program.spotLightLocations_[i];

				locations.position_ = glGetUniformLocation(program.id_, (prefix + "position").c_str());
				locations.direction_ = glGetUniformLocation(program.id_, (prefix + "direction").c_str());
				locations.color_ = glGetUniformLocation(program.id_, (prefix + "color").c_str());
				locations.intensity_ = glGetUniformLocation(program.id_, (prefix + "intensity").c_str());
				locations.range_ = glGetUniformLocation(program.id_, (prefix + "range").c_str());
				locations.innerConeCos_ = glGetUniformLocation(program.id_, (prefix + "innerConeCos").c_str());
				locations.outerConeCos_ = glGetUniformLocation(program.id_, (prefix + "outerConeCos").c_str());
			}
		}

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

bool Renderer::beginPass(const Camera& camera, int x, int y, int width, int height, float time) {
	if (passActive_) {
		throw std::logic_error("Renderer::beginPass: a pass is already active");
	}

	if (width <= 0 || height <= 0) {
		return false;
	}

	const float aspect = static_cast<float>(width) / static_cast<float>(height);

	viewProjection_ = camera.getViewProjectionMatrix(aspect);
	viewPosition_ = camera.getPosition();
	passTime_ = time;
	passResolutionX_ = static_cast<float>(width);
	passResolutionY_ = static_cast<float>(height);

	glViewport(x, y, width, height);

	lights_.clear();
	resetBindingCache();
	passActive_ = true;
	return true;
}

void Renderer::endPass() {
	glBindVertexArray(0);
	glUseProgram(0);

	for (GLuint unit = 0; unit < 4; ++unit) {
		glBindSampler(unit, 0);
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);

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

		if (program.viewPositionLocation_ != -1) {
			glUniform3f(program.viewPositionLocation_, viewPosition_.x_, viewPosition_.y_, viewPosition_.z_);
		}

		if (program.timeLocation_ != -1) {
			glUniform1f(program.timeLocation_, passTime_);
		}

		if (program.resolutionLocation_ != -1) {
			glUniform2f(program.resolutionLocation_, passResolutionX_, passResolutionY_);
		}

		uploadLights(program);

		boundProgram_ = program.id_;
	}

	glUniformMatrix4fv(program.modelLocation_, 1, GL_FALSE, model.data());

	if (program.colorLocation_ != -1) {
		glUniform4f(program.colorLocation_, material.color_.r_, material.color_.g_, material.color_.b_, 1.0f);
	}

	if (program.uvScaleLocation_ != -1) {
		glUniform2f(program.uvScaleLocation_, material.uvScaleU_, material.uvScaleV_);
	}

	if (program.hasTextureLocation_ != -1) {
		glUniform1i(program.hasTextureLocation_, material.albedoTexture_ != 0 ? GL_TRUE : GL_FALSE);
	}

	if (program.textureLocation_ != -1) {
		glUniform1i(program.textureLocation_, 0);
	}

	if (program.hasNormalTextureLocation_ != -1) {
		glUniform1i(program.hasNormalTextureLocation_, material.normalTexture_ != 0 ? GL_TRUE : GL_FALSE);
	}

	if (program.normalTextureLocation_ != -1) {
		glUniform1i(program.normalTextureLocation_, 1);
	}

	if (program.hasRoughnessTextureLocation_ != -1) {
		glUniform1i(program.hasRoughnessTextureLocation_, material.roughnessTexture_ != 0 ? GL_TRUE : GL_FALSE);
	}

	if (program.roughnessTextureLocation_ != -1) {
		glUniform1i(program.roughnessTextureLocation_, 2);
	}

	if (program.hasMetallicTextureLocation_ != -1) {
		glUniform1i(program.hasMetallicTextureLocation_, material.metallicTexture_ != 0 ? GL_TRUE : GL_FALSE);
	}

	if (program.metallicTextureLocation_ != -1) {
		glUniform1i(program.metallicTextureLocation_, 3);
	}

	if (program.roughnessLocation_ != -1) {
		glUniform1f(program.roughnessLocation_, material.roughness_);
	}

	if (program.metallicLocation_ != -1) {
		glUniform1f(program.metallicLocation_, material.metallic_);
	}

	const GLuint textures[] = {
		material.albedoTexture_,
		material.normalTexture_,
		material.roughnessTexture_,
		material.metallicTexture_
	};

	GLuint sampler = 0;
	for (GLuint unit = 0; unit < 4; ++unit) {
		if (textures[unit] != 0 && sampler == 0) {
			sampler = getOrCreateSampler(material.wrapS_, material.wrapT_);
		}

		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, textures[unit]);
		glBindSampler(unit, textures[unit] != 0 ? sampler : 0);
	}
	glActiveTexture(GL_TEXTURE0);

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

void Renderer::uploadLights(const ShaderProgram& program) const {
	const bool hasDirectionalLights = program.directionalLightCountLocation_ != -1;
	const bool hasPointLights = program.pointLightCountLocation_ != -1;
	const bool hasSpotLights = program.spotLightCountLocation_ != -1;

	if (!hasDirectionalLights && !hasPointLights && !hasSpotLights) {
		return;
	}

	int directionalLightCount = 0;
	int pointLightCount = 0;
	int spotLightCount = 0;

	for (const EngineRender::LightData& light : lights_) {
		if (light.type_ == EngineRender::LightType::Directional) {
			++directionalLightCount;
		}
		else if (light.type_ == EngineRender::LightType::Point) {
			++pointLightCount;
		}
		else if (light.type_ == EngineRender::LightType::Spot) {
			++spotLightCount;
		}
	}

	if (hasDirectionalLights && directionalLightCount > maxDirectionalLights_) {
		throw std::runtime_error("Renderer::uploadLights: directional light count exceeds shader capacity");
	}
	if (hasPointLights && pointLightCount > maxPointLights_) {
		throw std::runtime_error("Renderer::uploadLights: point light count exceeds shader capacity");
	}
	if (hasSpotLights && spotLightCount > maxSpotLights_) {
		throw std::runtime_error("Renderer::uploadLights: spotlight count exceeds shader capacity");
	}

	int directionalIndex = 0;
	int pointIndex = 0;
	int spotIndex = 0;

	for (const EngineRender::LightData& light : lights_) {
		if (hasDirectionalLights && light.type_ == EngineRender::LightType::Directional) {
			const auto& locations = program.directionalLightLocations_[directionalIndex++];

			glUniform3f(locations.direction_, light.direction_.x_, light.direction_.y_, light.direction_.z_);
			glUniform3f(locations.color_, light.color_.r_, light.color_.g_, light.color_.b_);
			glUniform1f(locations.intensity_, light.intensity_);
		}
		else if (hasPointLights && light.type_ == EngineRender::LightType::Point) {
			const auto& locations = program.pointLightLocations_[pointIndex++];

			glUniform3f(locations.position_, light.position_.x_, light.position_.y_, light.position_.z_);
			glUniform3f(locations.color_, light.color_.r_, light.color_.g_, light.color_.b_);
			glUniform1f(locations.intensity_, light.intensity_);
			glUniform1f(locations.range_, light.range_);
		}
		else if (hasSpotLights && light.type_ == EngineRender::LightType::Spot) {
			const auto& locations = program.spotLightLocations_[spotIndex++];

			glUniform3f(locations.position_, light.position_.x_, light.position_.y_, light.position_.z_);
			glUniform3f(locations.direction_, light.direction_.x_, light.direction_.y_, light.direction_.z_);
			glUniform3f(locations.color_, light.color_.r_, light.color_.g_, light.color_.b_);
			glUniform1f(locations.intensity_, light.intensity_);
			glUniform1f(locations.range_, light.range_);
			glUniform1f(locations.innerConeCos_, light.innerConeCos_);
			glUniform1f(locations.outerConeCos_, light.outerConeCos_);
		}
	}

	if (hasDirectionalLights) {
		glUniform1i(program.directionalLightCountLocation_, directionalLightCount);
	}
	if (hasPointLights) {
		glUniform1i(program.pointLightCountLocation_, pointLightCount);
	}
	if (hasSpotLights) {
		glUniform1i(program.spotLightCountLocation_, spotLightCount);
	}
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
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	const bool supportsAnisotropy =
		glutExtensionSupported("GL_EXT_texture_filter_anisotropic") ||
		glutExtensionSupported("GL_ARB_texture_filter_anisotropic");

	if (supportsAnisotropy) {
		// These extension constants are absent from the bundled GLAD header.
		constexpr GLenum textureMaxAnisotropy = 0x84FE;
		constexpr GLenum maxTextureMaxAnisotropy = 0x84FF;

		GLfloat maximumAnisotropy = 1.0f;
		glGetFloatv(maxTextureMaxAnisotropy, &maximumAnisotropy);

		const GLfloat anisotropy = maximumAnisotropy < 8.0f ? maximumAnisotropy : 8.0f;
		glSamplerParameterf(sampler, textureMaxAnisotropy, anisotropy);
	}

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

void Renderer::submitLight(const EngineRender::LightData& light) {
	if (!passActive_) {
		throw std::logic_error("Renderer::submitLight: no active render pass");
	}

	lights_.push_back(light);
}