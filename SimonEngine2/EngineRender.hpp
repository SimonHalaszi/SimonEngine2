#ifndef ENGINE_RENDER_HPP
#define ENGINE_RENDER_HPP

#include <string>

#include "EngineMath.hpp"
#include "EngineUtil.hpp"
#include "GraphicsAPI.hpp"

namespace EngineRender {
	class GPUMesh {
	public:
		GPUMesh() = default;

		~GPUMesh() {
			release();
		}

		// Life cycle and creating of GPUMeshes is not handled by programmer
		GPUMesh(const GPUMesh&) = delete;
		GPUMesh& operator=(const GPUMesh&) = delete;
		GPUMesh(GPUMesh&&) = delete;
		GPUMesh& operator=(GPUMesh&&) = delete;

		void release() {
			if (vao_ != 0) {
				glDeleteVertexArrays(1, &vao_);
				vao_ = 0;
			}

			if (vertexBuffer_ != 0) {
				glDeleteBuffers(1, &vertexBuffer_);
				vertexBuffer_ = 0;
			}

			if (faceIndexBuffer_ != 0) {
				glDeleteBuffers(1, &faceIndexBuffer_);
				faceIndexBuffer_ = 0;
			}

			if (edgeIndexBuffer_ != 0) {
				glDeleteBuffers(1, &edgeIndexBuffer_);
				edgeIndexBuffer_ = 0;
			}

			faceIndexCount_ = 0;
			edgeIndexCount_ = 0;
		}

		GLuint vao_ = 0; // Vertex Array Object, vertex layout and associated buffers

		GLuint vertexBuffer_ = 0; // Vertex positions
		GLuint faceIndexBuffer_ = 0; // Indices for filled geometry
		GLuint edgeIndexBuffer_ = 0; // Indices for outlines

		// Number of indexes to draw not actual count of faces or edges
		GLsizei faceIndexCount_ = 0;
		GLsizei edgeIndexCount_ = 0;
	};

	struct Material {
		EngineUtil::ColorRGB color_;
		std::string vertPath_;
		std::string fragPath_;
		std::string albedoTexturePath_;
		std::string normalTexturePath_;
		std::string roughnessTexturePath_;
		std::string metallicTexturePath_;

		std::string shader_;
		GLuint albedoTexture_ = 0;
		GLuint normalTexture_ = 0;
		GLuint roughnessTexture_ = 0;
		GLuint metallicTexture_ = 0;
		float uvScaleU_ = 1.0f;
		float uvScaleV_ = 1.0f;
		float roughness_ = 0.5f;
		float metallic_ = 0.0f;
		GLenum wrapS_ = GL_REPEAT;
		GLenum wrapT_ = GL_REPEAT;

		Material(
			const EngineUtil::ColorRGB& color = { 255, 255, 255 },
			float roughness = 0.5f,
			float metallic = 0.0f,
			const std::string& vertPath = "../Shaders/Basic.vert",
			const std::string& fragPath = "../Shaders/Color.frag",
			const std::string& albedoTexturePath = {},
			const std::string& normalTexturePath = {},
			const std::string& roughnessTexturePath = {},
			const std::string& metallicTexturePath = {},
			float scaleU = 1.0f,
			float scaleV = 1.0f,
			GLenum wrapS = GL_REPEAT,
			GLenum wrapT = GL_REPEAT
		)
			: 
			color_(color), roughness_(roughness), metallic_(metallic),
			vertPath_(vertPath), fragPath_(fragPath),
			albedoTexturePath_(albedoTexturePath), normalTexturePath_(normalTexturePath),
			roughnessTexturePath_(roughnessTexturePath), metallicTexturePath_(metallicTexturePath),
			uvScaleU_(scaleU), uvScaleV_(scaleV), wrapS_(wrapS), wrapT_(wrapT) 
		{}

		void init();
	};

	void upload(const EngineUtil::Mesh& CPUMesh, GPUMesh& outGPUMesh);

	enum class LightType {
		Directional,
		Point,
		Spot
	};

	struct LightData {
		LightType type_ = LightType::Directional;
		EngineUtil::ColorRGB color_;
		float intensity_ = 1.0f;

		EngineMath::Vector3 position_;
		EngineMath::Vector3 direction_ = { 0.0f, 0.0f, -1.0f };

		float range_ = 10.0f;
		float innerConeCos_ = 1.0f;
		float outerConeCos_ = 1.0f;
	};

	struct CubeMapData {
		EngineMath::Vector3 position_;
		GLuint textureID_ = 0;
	};
}

#endif 
