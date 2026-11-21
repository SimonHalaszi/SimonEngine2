#ifndef TEXTURE_REGISTRY_HPP
#define TEXTURE_REGISTRY_HPP

#include "GraphicsAPI.hpp"
#include <FreeImage/FreeImage.h>

#include <unordered_map>
#include <map>
#include <array>
#include <string>
#include <iostream>

// Registry for textures

class TextureRegistry {
	public:
		static TextureRegistry& getInstance() {
			static TextureRegistry instance;
			return instance;
		}

		GLuint loadTexture(std::string filepath);
		void unloadTexture(std::string filepath);

		// Face order: right (+X), left (-X), up (+Y), down (-Y), towards (+Z), aways (-Z).
		GLuint loadCubeMap(const std::array<std::string, 6>& facePaths);

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
		std::map<std::array<std::string, 6>, GLuint> cubeMapMap_;
};

#endif