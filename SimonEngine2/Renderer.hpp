#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include <unordered_map>
#include <map>
#include <stdexcept>
#include <vector>
#include <array>

#include "EngineMath.hpp"
#include "EngineUtil.hpp"
#include "EngineShader.hpp"
#include "EngineRender.hpp"

class Renderer {
	public:
		enum class DrawMode { Solid, Edges };

		static Renderer& getInstance() {
			static Renderer instance;
			return instance;
		}

		void loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
		std::string getOrLoadShader(const std::string& vertPath, const std::string& fragPath);
		
		bool beginPass(const class Camera& camera,
			int x, int y,
			int width, int height,
			float time
		);

		void endPass();

		void drawMesh(const EngineRender::GPUMesh& mesh, const EngineRender::Material& material, const EngineMath::Matrix4& model, DrawMode mode = DrawMode::Solid) const;
		void submitLight(const EngineRender::LightData& light);

		void clear();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;
	private:
		Renderer() = default;

		static constexpr int maxDirectionalLights_ = 2;

		struct DirectionalLightLocations {
			GLint direction_ = -1;
			GLint color_ = -1;
			GLint intensity_ = -1;
		};

		static constexpr int maxPointLights_ = 8;

		struct PointLightLocations {
			GLint position_ = -1;
			GLint color_ = -1;
			GLint intensity_ = -1;
			GLint range_ = -1;
		};

		static constexpr int maxSpotLights_ = 8;

		struct SpotLightLocations {
			GLint position_ = -1;
			GLint direction_ = -1;
			GLint color_ = -1;
			GLint intensity_ = -1;
			GLint range_ = -1;
			GLint innerConeCos_ = -1;
			GLint outerConeCos_ = -1;
		};

		struct ShaderProgram {
			GLuint id_ = 0;
			GLint viewProjLocation_ = -1;
			GLint modelLocation_ = -1;
			GLint colorLocation_ = -1;
			GLint textureLocation_ = -1;
			GLint timeLocation_ = -1;
			GLint resolutionLocation_ = -1;
			GLint uvScaleLocation_ = -1;

			GLint hasTextureLocation_ = -1;

			GLint directionalLightCountLocation_ = -1;
			std::array<DirectionalLightLocations, maxDirectionalLights_> directionalLightLocations_;

			GLint pointLightCountLocation_ = -1;
			std::array<PointLightLocations, maxPointLights_> pointLightLocations_;

			GLint spotLightCountLocation_ = -1;
			std::array<SpotLightLocations, maxSpotLights_> spotLightLocations_;
		};

		void uploadLights(const ShaderProgram& program) const;

		EngineMath::Matrix4 viewProjection_ = EngineMath::Matrix4::identity();
		bool passActive_ = false;

		float passTime_ = 0.0f;
		float passResolutionX_ = 1.0f;
		float passResolutionY_ = 1.0f;

		std::unordered_map<std::string, ShaderProgram> programRegistry_;

		mutable std::map<std::pair<GLenum, GLenum>, GLuint> samplerRegistry_;
		GLuint getOrCreateSampler(GLenum wrapS, GLenum wrapT) const;

		std::vector<EngineRender::LightData> lights_;

		~Renderer() {}

		mutable GLuint boundProgram_ = 0;
		mutable GLuint boundVAO_ = 0;
		mutable GLuint boundElementBuffer_ = 0;

		void resetBindingCache() const {
			boundProgram_ = 0;
			boundVAO_ = 0;
			boundElementBuffer_ = 0;
		}
};

#endif