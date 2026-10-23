#include "Screenshot.hpp"

#include <GL/freeglut.h>

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

namespace Screenshot {

	namespace {
		// BMP headers are little-endian. Writing byte by byte means this works the same on
		// every compiler and CPU without any #pragma pack shenanigans.
		void put16(std::ofstream& out, std::uint16_t v) {
			out.put(static_cast<char>(v & 0xFF));
			out.put(static_cast<char>((v >> 8) & 0xFF));
		}

		void put32(std::ofstream& out, std::uint32_t v) {
			for (int i = 0; i < 4; ++i) {
				out.put(static_cast<char>((v >> (8 * i)) & 0xFF));
			}
		}

		std::string nextFreeName() {
			for (int i = 1; i < 10000; ++i) {
				std::ostringstream name;
				name << "SE2_CHUCK_CLIP_" << std::setfill('0') << std::setw(4) << i << ".bmp";
				std::ifstream exists(name.str().c_str());
				if (!exists) {
					return name.str();
				}
			}
			return std::string();
		}
	}

	std::string saveBmp() {
		const int width = glutGet(GLUT_WINDOW_WIDTH);
		const int height = glutGet(GLUT_WINDOW_HEIGHT);
		if (width <= 0 || height <= 0) {
			return std::string();
		}

		// Tightly packed RGB rows please (OpenGL's default pads rows to 4 bytes)
		std::vector<unsigned char> pixels(static_cast<std::size_t>(width) * height * 3);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

		const std::string name = nextFreeName();
		if (name.empty()) {
			return std::string();
		}

		std::ofstream out(name.c_str(), std::ios::binary);
		if (!out) {
			return std::string();
		}

		// ...but BMP DOES want every row padded to a multiple of 4 bytes
		const std::uint32_t rowBytes = static_cast<std::uint32_t>(width) * 3u;
		const std::uint32_t paddedRow = (rowBytes + 3u) & ~3u;
		const std::uint32_t imageBytes = paddedRow * static_cast<std::uint32_t>(height);

		// ---- BITMAPFILEHEADER (14 bytes)
		out.put('B');
		out.put('M');
		put32(out, 14u + 40u + imageBytes); // total file size
		put16(out, 0);                      // reserved
		put16(out, 0);                      // reserved
		put32(out, 14u + 40u);              // where the pixels start

		// ---- BITMAPINFOHEADER (40 bytes)
		put32(out, 40u);                                   // header size
		put32(out, static_cast<std::uint32_t>(width));
		put32(out, static_cast<std::uint32_t>(height));    // positive = rows stored bottom-up
		put16(out, 1);                                     // color planes (always 1)
		put16(out, 24);                                    // bits per pixel
		put32(out, 0u);                                    // no compression
		put32(out, imageBytes);
		put32(out, 2835u);                                 // 72 DPI, in pixels per meter
		put32(out, 2835u);
		put32(out, 0u);                                    // palette colors (none)
		put32(out, 0u);

		// ---- pixels: BMP wants BGR, OpenGL gave us RGB, so swap on the way out
		const char padding[3] = { 0, 0, 0 };
		for (int y = 0; y < height; ++y) {
			const unsigned char* row = &pixels[static_cast<std::size_t>(y) * rowBytes];
			for (int x = 0; x < width; ++x) {
				out.put(static_cast<char>(row[x * 3 + 2]));
				out.put(static_cast<char>(row[x * 3 + 1]));
				out.put(static_cast<char>(row[x * 3 + 0]));
			}
			out.write(padding, paddedRow - rowBytes);
		}

		return out ? name : std::string();
	}
}
