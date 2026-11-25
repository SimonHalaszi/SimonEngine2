#include "TextureRegistry.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

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

GLuint TextureRegistry::loadCubeMap(const std::string& exrFilepath, float rotationDegrees, float exposure) {
	if (!std::isfinite(rotationDegrees) || !std::isfinite(exposure) || exposure < 0.0f) {
		std::cout << "TextureRegistry::loadCubeMap : Invalid rotation or exposure" << std::endl;
		return 0;
	}

	auto key = std::make_tuple(exrFilepath, rotationDegrees, exposure);
	auto it = cubeMapMap_.find(key);
	if (it != cubeMapMap_.end()) {
		return it->second;
	}

	using ImagePtr = std::unique_ptr<FIBITMAP, decltype(&FreeImage_Unload)>;
	ImagePtr image(FreeImage_Load(FIF_EXR, exrFilepath.c_str(), 0), &FreeImage_Unload);

	if (!image) {
		std::cout << "TextureRegistry::loadCubeMap : Failed to load " << exrFilepath << std::endl;
		return 0;
	}

	FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(image.get());
	if (imageType != FIT_RGBF && imageType != FIT_RGBAF && imageType != FIT_FLOAT) {
		std::cout << "TextureRegistry::loadCubeMap : Unsupported EXR image type " << exrFilepath << std::endl;
		return 0;
	}

	int width = static_cast<int>(FreeImage_GetWidth(image.get()));
	int height = static_cast<int>(FreeImage_GetHeight(image.get()));

	if (width < 4 || height < 2 || width / 2 != height || width % 2 != 0) {
		std::cout << "TextureRegistry::loadCubeMap : Expected a 2:1 equirectangular image " << exrFilepath << std::endl;
		return 0;
	}

	int faceSize = width / 4;
	GLint maxSize = 0;
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxSize);

	if (faceSize > maxSize) {
		std::cout << "TextureRegistry::loadCubeMap : Cubemap exceeds maximum texture size" << std::endl;
		return 0;
	}

	struct Pixel {
		float red, green, blue, alpha;
	};

	auto readPixel = [&](int x, int y) -> Pixel {
		BYTE* row = FreeImage_GetScanLine(image.get(), y);

		if (imageType == FIT_RGBAF) {
			const FIRGBAF& pixel = reinterpret_cast<const FIRGBAF*>(row)[x];
			return { pixel.red, pixel.green, pixel.blue, pixel.alpha };
		}

		if (imageType == FIT_RGBF) {
			const FIRGBF& pixel = reinterpret_cast<const FIRGBF*>(row)[x];
			return { pixel.red, pixel.green, pixel.blue, 1.0f };
		}

		float value = reinterpret_cast<const float*>(row)[x];
		return { value, value, value, 1.0f };
		};

	auto interpolate = [](const Pixel& a, const Pixel& b, float amount) -> Pixel {
		return {
			a.red + (b.red - a.red) * amount,
			a.green + (b.green - a.green) * amount,
			a.blue + (b.blue - a.blue) * amount,
			a.alpha + (b.alpha - a.alpha) * amount
		};
		};

	auto toDisplayByte = [exposure](float value) -> BYTE {
		double exposed = static_cast<double>(std::max(value, 0.0f)) * static_cast<double>(exposure);
		double mapped = exposed / (1.0 + exposed);
		double srgb = mapped <= 0.0031308 ? 12.92 * mapped : 1.055 * std::pow(mapped, 1.0 / 2.4) - 0.055;

		return static_cast<BYTE>(std::lround(std::clamp(srgb, 0.0, 1.0) * 255.0));
		};

	const float pi = 3.14159265358979323846f;
	float radians = std::fmod(rotationDegrees, 360.0f) * pi / 180.0f;
	float cosine = std::cos(radians);
	float sine = std::sin(radians);

	std::vector<BYTE> pixels(static_cast<std::size_t>(faceSize) * faceSize * 4);

	GLint previousBinding = 0;
	GLint previousAlignment = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &previousBinding);
	glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);

	GLuint texID = 0;
	glGenTextures(1, &texID);

	if (texID == 0) {
		std::cout << "TextureRegistry::loadCubeMap : Failed to create cubemap texture" << std::endl;
		return 0;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	auto restoreState = [&]() {
		glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
		glBindTexture(GL_TEXTURE_CUBE_MAP, static_cast<GLuint>(previousBinding));
		};

	for (int face = 0; face < 6; ++face) {
		for (int y = 0; y < faceSize; ++y) {
			for (int x = 0; x < faceSize; ++x) {
				float s = 2.0f * (x + 0.5f) / faceSize - 1.0f;
				float t = 2.0f * (y + 0.5f) / faceSize - 1.0f;
				float dx, dy, dz;

				switch (face) {
				case 0: dx = 1.0f;  dy = -t;    dz = -s;   break;
				case 1: dx = -1.0f; dy = -t;    dz = s;    break;
				case 2: dx = s;     dy = 1.0f;  dz = t;    break;
				case 3: dx = s;     dy = -1.0f; dz = -t;   break;
				case 4: dx = s;     dy = -t;    dz = 1.0f; break;
				default: dx = -s;   dy = -t;    dz = -1.0f; break;
				}

				float rotatedX = cosine * dx - sine * dz;
				float rotatedZ = sine * dx + cosine * dz;
				float length = std::sqrt(rotatedX * rotatedX + dy * dy + rotatedZ * rotatedZ);

				float u = 0.5f + std::atan2(rotatedZ, rotatedX) / (2.0f * pi);
				float v = 0.5f + std::asin(std::clamp(dy / length, -1.0f, 1.0f)) / pi;

				float imageX = u * width - 0.5f;
				float imageY = std::clamp(v * height - 0.5f, 0.0f, static_cast<float>(height - 1));

				int x0 = static_cast<int>(std::floor(imageX));
				int y0 = static_cast<int>(std::floor(imageY));
				float xBlend = imageX - x0;
				float yBlend = imageY - y0;

				x0 = (x0 % width + width) % width;
				int x1 = (x0 + 1) % width;
				int y1 = std::min(y0 + 1, height - 1);

				Pixel bottom = interpolate(readPixel(x0, y0), readPixel(x1, y0), xBlend);
				Pixel top = interpolate(readPixel(x0, y1), readPixel(x1, y1), xBlend);
				Pixel pixel = interpolate(bottom, top, yBlend);

				if (!std::isfinite(pixel.red) || !std::isfinite(pixel.green) ||
					!std::isfinite(pixel.blue) || !std::isfinite(pixel.alpha)) {
					std::cout << "TextureRegistry::loadCubeMap : Invalid pixel in " << exrFilepath << std::endl;
					restoreState();
					glDeleteTextures(1, &texID);
					return 0;
				}

				std::size_t index = (static_cast<std::size_t>(y) * faceSize + x) * 4;
				pixels[index] = toDisplayByte(pixel.red);
				pixels[index + 1] = toDisplayByte(pixel.green);
				pixels[index + 2] = toDisplayByte(pixel.blue);
				pixels[index + 3] = static_cast<BYTE>(std::lround(std::clamp(pixel.alpha, 0.0f, 1.0f) * 255.0f));
			}
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGBA8, faceSize, faceSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	restoreState();

	try {
		cubeMapMap_.emplace(key, texID);
	}
	catch (...) {
		glDeleteTextures(1, &texID);
		throw;
	}

	std::cout << "TextureRegistry::loadCubeMap : CubeMap image loaded from file path " << exrFilepath << " " << width << " X " << height << std::endl;

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
