#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include <unordered_map>
#include <stdexcept>

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

		void shutdown();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;
	private:
		Renderer() = default;

		struct ShaderProgram {
			GLuint id_ = 0;
			GLint viewProjLocation_ = -1;
			GLint modelLocation_ = -1;
			GLint colorLocation_ = -1;
			GLint timeLocation_ = -1;
			GLint resolutionLocation_ = -1;
		};

		EngineMath::Matrix4 viewProjection_ = EngineMath::Matrix4::identity();
		bool passActive_ = false;

		float passTime_ = 0.0f;
		float passResolutionX_ = 1.0f;
		float passResolutionY_ = 1.0f;

		std::unordered_map<std::string, ShaderProgram> programRegistry_;

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