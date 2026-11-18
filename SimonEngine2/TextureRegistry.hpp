#ifndef TEXTURE_REGISTRY_HPP
#define TEXTURE_REGISTRY_HPP

#include "GraphicsAPI.hpp"
#include <FreeImage/FreeImage.h>

#include <unordered_map>
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

		void clearRegistry() {
			for (auto& pair : textureMap_) {
				glDeleteTextures(1, &pair.second);
			}
			std::cout << "TextureRegistry::clearRegistry : Deleted all OpenGL textures. Cleared TextureRegistry" << std::endl;
			textureMap_.clear();
		}

		GLuint getTextureID(std::string filepath) const;

		TextureRegistry(const TextureRegistry&) = delete;
		TextureRegistry& operator=(const TextureRegistry&) = delete;
		TextureRegistry(const TextureRegistry&&) = delete;
		TextureRegistry& operator=(const TextureRegistry&&) = delete;

	private:
		TextureRegistry() {}

		~TextureRegistry() {
			for (auto& pair : textureMap_) {
				glDeleteTextures(1, &pair.second);
			}
		}

		// Registry
		std::unordered_map<std::string, GLuint> textureMap_;
};

#endif