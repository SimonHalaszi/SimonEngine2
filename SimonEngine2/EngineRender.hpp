#ifndef ENGINE_RENDER_HPP
#define ENGINE_RENDER_HPP

#include <stdexcept>
#include <string>
#include <limits>

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
		std::string shader_ = "color";
		EngineUtil::ColorRGB color_;
	};

	void upload(const EngineUtil::Mesh& CPUMesh, GPUMesh& outGPUMesh);
}

#endif 
