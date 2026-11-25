#ifndef TEXTURE_REGISTRY_HPP
#define TEXTURE_REGISTRY_HPP

#include "GraphicsAPI.hpp"
#include <FreeImage/FreeImage.h>

#include <unordered_map>
#include <map>
#include <array>
#include <string>
#include <iostream>
#include <tuple>

// Registry for textures

class TextureRegistry {
	public:
		static TextureRegistry& getInstance() {
			static TextureRegistry instance;
			return instance;
		}

		GLuint loadTexture(std::string filepath);
		void unloadTexture(std::string filepath);

		GLuint loadCubeMap(const std::string& exrFilepath, float rotationDegrees = 0.0f, float exposure = 1.0f);

		void clearRegistry();

		GLuint getTextureID(std::string filepath) const;

		TextureRegistry(const TextureRegistry&) = delete;
		TextureRegistry& operator=(const TextureRegistry&) = delete;
		TextureRegistry(const TextureRegistry&&) = delete;
		TextureRegistry& operator=(const TextureRegistry&&) = delete;

	private:
		TextureRegistry() {}

		~TextureRegistry();

		// Registry
		std::unordered_map<std::string, GLuint> textureMap_;
		std::map<std::tuple<std::string, float, float>, GLuint> cubeMapMap_;
};

#endif