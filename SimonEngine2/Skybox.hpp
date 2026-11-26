#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include "GraphicsAPI.hpp"

#include "Object.hpp"

class Skybox : public Object {
	public:
		Skybox(GLuint textureID) : textureID_(textureID) {}

		GLuint getTextureID() const { return textureID_; }

		void submitSkyboxInfo() const;

	protected:
		void onStart() override;
		void onDestruction() override;

	private:
		GLuint textureID_ = 0;
};

#endif