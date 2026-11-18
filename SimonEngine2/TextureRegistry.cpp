#include "TextureRegistry.hpp"

// Changed to load one texture at a time given a string filepath, adds to registry, also returns id
// Also fixed to support the alpha channel, and handle the RGBA to BGRA conversion that FreeImage doesnt do by default
GLuint TextureRegistry::loadTexture(std::string filepath) {
	auto it = textureMap_.find(filepath);
	if (it != textureMap_.end()) {
		return it->second;
	}

	void* imgData = nullptr; // Pointer to image color data read from the file.
	int imgWidth = 0; // The width of the image that was read.
	int imgHeight = 0; // The height.
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filepath.c_str());
	if (format == FIF_UNKNOWN) {
		std::cout << "TextureRegistry::loadTexture : Unknown file path type for texture image file " << filepath << std::endl;
		return 0;
	}
	FIBITMAP* imageFile = FreeImage_Load(format, filepath.c_str(), 0); // Read image from file.
	if (!imageFile) {
		std::cout << "TextureRegistry::loadTexture : Failed to load image " << filepath << std::endl;
		return 0;
	}

	FIBITMAP* image32bit = FreeImage_ConvertTo32Bits(imageFile);
	FreeImage_Unload(imageFile);

	int width = FreeImage_GetWidth(image32bit);
	int height = FreeImage_GetHeight(image32bit);

	BYTE* pixels = FreeImage_GetBits(image32bit); // Internal pixel data of image32bit

	GLuint texID;
	glGenTextures(1, &texID);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (pixels) {
		std::cout << "TextureRegistry::loadTexture : Texture image loaded from file path " << filepath << " " << width << " X " << height << std::endl;

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	else {
		std::cout << "TextureRegistry::loadTexture : Failed to get texture data from file path " << filepath << std::endl;
	}

	FreeImage_Unload(image32bit); // Also cleans up pixels

	// Get added to textureMap registry
	textureMap_[filepath] = texID;
	// Also returned
	return texID;
}

void TextureRegistry::unloadTexture(std::string filepath) {
	auto it = textureMap_.find(filepath);
	if (it == textureMap_.end()) {
		std::cout << "TextureRegistry::unloadTexture : No texture from file path " << filepath << " to unload." << std::endl;
		return;
	}

	GLuint texID = it->second;

	textureMap_.erase(filepath);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &texID);
}

GLuint TextureRegistry::getTextureID(std::string filepath) const {
	auto it = textureMap_.find(filepath);
	if (it == textureMap_.end()) {
		std::cout << "TextureRegistry::getTextureID : Couldnt get texture that came from file path " << filepath << std::endl;
		return 0;
	}
	else {
		return textureMap_.at(filepath);
	}
}