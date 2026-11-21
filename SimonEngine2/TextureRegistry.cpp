#include "TextureRegistry.hpp"

// Changed to load one texture at a time given a string filepath, adds to registry, also returns id
// Also fixed to support the alpha channel, and handle the RGBA to BGRA conversion that FreeImage doesnt do by default
GLuint TextureRegistry::loadTexture(std::string filepath) {
	auto it = textureMap_.find(filepath);
	if (it != textureMap_.end()) {
		return it->second;
	}

	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filepath.c_str());
	if (format == FIF_UNKNOWN) {
		std::cout << "TextureRegistry::loadTexture : Unknown file path type for texture image file " << filepath << std::endl;
		return 0;
	}

	FIBITMAP* imageFile = FreeImage_Load(format, filepath.c_str(), 0);
	if (!imageFile) {
		std::cout << "TextureRegistry::loadTexture : Failed to load image " << filepath << std::endl;
		return 0;
	}

	FIBITMAP* image32bit = FreeImage_ConvertTo32Bits(imageFile);
	FreeImage_Unload(imageFile);

	if (!image32bit) {
		std::cout << "TextureRegistry::loadTexture : Failed to convert image " << filepath << std::endl;
		return 0;
	}

	int width = FreeImage_GetWidth(image32bit);
	int height = FreeImage_GetHeight(image32bit);
	BYTE* pixels = FreeImage_GetBits(image32bit);

	if (!pixels || width <= 0 || height <= 0) {
		std::cout << "TextureRegistry::loadTexture : Invalid texture data from file path " << filepath << std::endl;
		FreeImage_Unload(image32bit);
		return 0;
	}

	GLuint texID = 0;
	glGenTextures(1, &texID);

	if (texID == 0) {
		std::cout << "TextureRegistry::loadTexture : Failed to create texture for " << filepath << std::endl;
		FreeImage_Unload(image32bit);
		return 0;
	}

	GLint previousBinding = 0;
	GLint previousAlignment = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBinding);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	FreeImage_Unload(image32bit);

	glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
	glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousBinding));

	try {
		textureMap_.emplace(filepath, texID);
	}
	catch (...) {
		glDeleteTextures(1, &texID);
		throw;
	}

	std::cout << "TextureRegistry::loadTexture : Texture image loaded from file path " << filepath << " " << width << " X " << height << std::endl;

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

GLuint TextureRegistry::loadCubeMap(const std::array<std::string, 6>& facePaths) {
	auto it = cubeMapMap_.find(facePaths);
	if (it != cubeMapMap_.end()) {
		return it->second;
	}

	GLint previousBinding = 0;
	GLint previousAlignment = 0;
	GLint maxSize = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &previousBinding);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxSize);

	GLuint texID = 0;
	glGenTextures(1, &texID);

	if (texID == 0) {
		std::cout << "TextureRegistry::loadCubeMap : Failed to create cubemap texture" << std::endl;
		return 0;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	int faceSize = 0;

	for (std::size_t i = 0; i < facePaths.size(); ++i) {
		const std::string& filepath = facePaths[i];
		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filepath.c_str());
		FIBITMAP* imageFile = nullptr;

		if (format != FIF_UNKNOWN) {
			imageFile = FreeImage_Load(format, filepath.c_str(), 0);
		}

		if (!imageFile) {
			std::cout << "TextureRegistry::loadCubeMap : Failed to load image " << filepath << std::endl;
			glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
			glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(previousBinding));
			glDeleteTextures(1, &texID);
			return 0;
		}

		FIBITMAP* image32bit = FreeImage_ConvertTo32Bits(imageFile);
		FreeImage_Unload(imageFile);

		if (!image32bit) {
			std::cout << "TextureRegistry::loadCubeMap : Failed to convert image " << filepath << std::endl;
			glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
			glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(previousBinding));
			glDeleteTextures(1, &texID);
			return 0;
		}

		if (!FreeImage_FlipVertical(image32bit)) {
			std::cout << "TextureRegistry::loadCubeMap : Failed to flip image " << filepath << std::endl;
			FreeImage_Unload(image32bit);
			glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
			glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(previousBinding));
			glDeleteTextures(1, &texID);
			return 0;
		}

		int width = FreeImage_GetWidth(image32bit);
		int height = FreeImage_GetHeight(image32bit);
		BYTE* pixels = FreeImage_GetBits(image32bit);

		if (!pixels || width <= 0 || width != height || width > maxSize || (faceSize != 0 && width != faceSize)) {
			std::cout << "TextureRegistry::loadCubeMap : Invalid or mismatched face " << filepath << std::endl;
			FreeImage_Unload(image32bit);
			glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
			glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(previousBinding));
			glDeleteTextures(1, &texID);
			return 0;
		}

		faceSize = width;

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, GL_RGBA8,
			width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

		FreeImage_Unload(image32bit);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
	glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(previousBinding));

	try {
		cubeMapMap_.emplace(facePaths, texID);
	}
	catch (...) {
		glDeleteTextures(1, &texID);
		throw;
	}

	return texID;
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

void TextureRegistry::clearRegistry() {
	for (auto& pair : textureMap_) {
		glDeleteTextures(1, &pair.second);
	}
	for (auto& pair : cubeMapMap_) {
		glDeleteTextures(1, &pair.second);
	}

	std::cout << "TextureRegistry::clearRegistry : Deleted all OpenGL textures. Cleared TextureRegistry" << std::endl;
	textureMap_.clear();
	cubeMapMap_.clear();
}

TextureRegistry::~TextureRegistry() {
	for (auto& pair : textureMap_) {
		glDeleteTextures(1, &pair.second);
	}
	for (auto& pair : cubeMapMap_) {
		glDeleteTextures(1, &pair.second);
	}
}
