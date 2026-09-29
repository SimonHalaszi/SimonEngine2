#ifndef ENGINE_UTIL_HPP
#define ENGINE_UTIL_HPP

// My little util library!

#include <vector>

namespace EngineUtil {
	class ColorRGB {
		public:
			ColorRGB() : r_(1.0f), g_(1.0f), b_(1.0f) {}
			ColorRGB(float r, float g, float b) : r_(r), g_(g), b_(b) {}
			ColorRGB(int r, int g, int b) : r_(iTof(r)), g_(iTof(g)), b_(iTof(b)) {}
			float r_, g_, b_;

			static float iTof(int value) {
				if (value < 0) {
					value = 0;
				}
				if (value > 255) {
					value = 255;
				}
				return static_cast<float>(value) / 255.0f;
			}

			static ColorRGB toGrayScale(const ColorRGB& c) {
				float base = 0.299f * c.r_ + 0.587f * c.g_ + 0.114f * c.b_;
				return ColorRGB(base, base, base);
			}
	};

	struct Vertex {
		float x_, y_, z_;
	};

	enum class Axis {
		X, Y, Z
	};

	class Mesh {
		public:
			Mesh() = default;
			std::vector<Vertex> vertices_;
			std::vector<int> faceIndices_;
			std::vector<int> edgeIndices_;
	};
}



#endif