#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <array>
#include <string>

#include "Object.hpp"
#include "GraphicsAPI.hpp"

class Skybox : public Object {
	public:
		Skybox(GLuint textureID) : textureID_(textureID) {}

		GLuint getTextureID() const { return textureID_; }

	protected:
		void onStart() override;
		void onDestruction() override;

	private:
		GLuint textureID_ = 0;
};

#endif