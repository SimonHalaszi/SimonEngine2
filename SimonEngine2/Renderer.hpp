#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "EngineMath.hpp"
#include "GPUMesh.hpp"
#include "EngineUtil.hpp"
#include "EngineShader.hpp"

class Renderer {
	public:
		static Renderer& getInstance() {
			static Renderer instance;
			return instance;
		}

		void loadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
		std::string getOrLoadShader(const std::string& vertPath, const std::string& fragPath);
		
		void drawMesh(const GPUMesh& mesh, const EngineUtil::Material& material, const EngineMath::Matrix4& model, const EngineMath::Matrix4& viewProjection) const;

		void shutdown();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(const Renderer&&) = delete;
		Renderer& operator=(const Renderer&&) = delete;
	private:
		Renderer() = default;

		struct ShaderProgram {
			GLuint id_ = 0;
			GLint viewProjLocation_ = -1;
			GLint modelLocation_ = -1;
			GLint colorLocation_ = -1;
		};

		std::unordered_map<std::string, ShaderProgram> programRegistry_;

		~Renderer() {}
};

#endif