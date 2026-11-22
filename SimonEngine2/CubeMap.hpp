#ifndef CUBE_MAP_HPP
#define CUBE_MAP_HPP

#include <array>
#include <string>

#include "Object.hpp"
#include "GraphicsAPI.hpp"

class CubeMap : public Object {
	public:
		CubeMap(const EngineMath::Transform& localTransform, GLuint textureID);

		GLuint getTextureID() const { return textureID_; }

		void submitCubeMapInfo() const;

	protected:
		void onStart() override;
		void onDestruction() override;

	private:
		GLuint textureID_ = 0;
};

#endif