#ifndef CUBE_MAP_HPP
#define CUBE_MAP_HPP

#include <array>
#include <string>

#include "Object.hpp"
#include "GraphicsAPI.hpp"

class CubeMap : public Object {
	public:
		CubeMap(const EngineMath::Transform& localTransform, GLuint textureID)
			: textureID_(textureID) {
			if (textureID_ == 0) {
				throw std::invalid_argument("CubeMap: texture ID cannot be zero");
			}

			localTransform_ = localTransform;
		}

		GLuint getTextureID() const { return textureID_; }
		void submitCubeMapInfo() const;

	protected:
		void onStart() override;
		void onDestruction() override;

	private:
		GLuint textureID_ = 0;
};

#endif