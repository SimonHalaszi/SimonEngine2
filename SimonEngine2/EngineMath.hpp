#ifndef ENGINE_MATH_HPP
#define ENGINE_MATH_HPP

#include <cmath>
#include <array>

// My little 3D math library!

namespace EngineMath {

	constexpr float PI = 3.14159265358979323846f;

	inline float degreesToRadians(float degrees) { return degrees * PI / 180.0f; }

	class Vector3 {
		public:
			float x_, y_, z_;

			Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x_(x), y_(y), z_(z) {}

			Vector3 operator+(const Vector3& other) const { return { x_ + other.x_, y_ + other.y_, z_ + other.z_ }; }
			Vector3 operator-(const Vector3& other) const { return { x_ - other.x_, y_ - other.y_, z_ - other.z_ }; }
			Vector3 operator*(float scalar) const { return { x_ * scalar, y_ * scalar, z_ * scalar }; }
			Vector3 operator*(const Vector3& other) const { return { x_ * other.x_, y_ * other.y_, z_ * other.z_ }; }

			Vector3& operator+=(const Vector3& other) { x_ += other.x_; y_ += other.y_; z_ += other.z_; return *this; }
			Vector3& operator-=(const Vector3& other) { x_ -= other.x_; y_ -= other.y_; z_ -= other.z_; return *this; }
			Vector3& operator*=(float scalar) { x_ *= scalar; y_ *= scalar; z_ *= scalar; return *this; }

			Vector3 operator-() const { return { -x_, -y_, -z_ }; }

			float dot(const Vector3& other) const { return x_ * other.x_ + y_ * other.y_ + z_ * other.z_; }

			Vector3 cross(const Vector3& other) const { 
				return { y_ * other.z_ - z_ * other.y_, z_ * other.x_ - x_ * other.z_, x_ * other.y_ - y_ * other.x_ }; 
			}

			float lengthSquared() const { return dot(*this); }
			float length() const { return std::sqrt(lengthSquared()); }

			Vector3 normalized() const {
				float len = length(); return len > 1e-8f ? (*this) * (1.0f / len) : Vector3{ 0,0,0 };
			}
	};

	inline Vector3 operator*(float scalar, const Vector3& vector) {	return vector * scalar; }

	class Quaternion {
		public:
			float x_, y_, z_, w_;

			Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) : x_(x), y_(y), z_(z), w_(w) {}

			static Quaternion identity() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }

			static Quaternion fromAxisAngle(const Vector3& axis, float radians) {
				float axisLengthSquared = axis.lengthSquared();

				// There is no meaningful rotation axis here.
				if (axisLengthSquared <= 1e-8f) { return Quaternion::identity(); }

				Vector3 n = axis * (1.0f / std::sqrt(axisLengthSquared));
				float half = radians * 0.5f;
				float sine = std::sin(half);

				return { n.x_ * sine, n.y_ * sine, n.z_ * sine, std::cos(half) };
			}

			// Expects Radians
			static Quaternion fromXYZ(float pitchX, float yawY, float rollZ) {
				Quaternion qx = fromAxisAngle({ 1.0f, 0.0f, 0.0f }, pitchX);
				Quaternion qy = fromAxisAngle({ 0.0f, 1.0f, 0.0f }, yawY);
				Quaternion qz = fromAxisAngle({ 0.0f, 0.0f, 1.0f }, rollZ);
				return qz * qy * qx;
			}

			// Hamilton Product
			Quaternion operator*(const Quaternion& b) const {
				return {
					w_* b.x_ + x_ * b.w_ + y_ * b.z_ - z_ * b.y_,
					w_* b.y_ - x_ * b.z_ + y_ * b.w_ + z_ * b.x_,
					w_* b.z_ + x_ * b.y_ - y_ * b.x_ + z_ * b.w_,
					w_* b.w_ - x_ * b.x_ - y_ * b.y_ - z_ * b.z_
				};
			}

			// Inverse, if normalized
			Quaternion conjugate() const { return { -x_, -y_, -z_, w_ }; }

			// Rotate a vector by this quaternion
			Vector3 rotate(const Vector3& v) const {
				Quaternion vq(v.x_, v.y_, v.z_, 0.0f);
				Quaternion result = (*this) * vq * conjugate();
				return { result.x_, result.y_, result.z_ };
			}

			Quaternion normalized() const {
				float lenSquared = x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_;

				if (lenSquared <= 1e-8f) { return Quaternion::identity(); }

				float invLen = 1.0f / std::sqrt(lenSquared);

				return { x_ * invLen, y_ * invLen, z_ * invLen, w_ * invLen };
			}
	};

	class Matrix4 {
		public:
			// column-major
			std::array<float, 16> m_{};

			Matrix4(const std::array<float, 16>& m = {}) : m_(m) {}

			const float* data() const { return m_.data(); }
			float* data() { return m_.data(); }

			static Matrix4 identity() {
				Matrix4 r;
				r.m_[0] = r.m_[5] = r.m_[10] = r.m_[15] = 1.0f;;
				return r;
			}

			static Matrix4 translation(const Vector3& t) {
				Matrix4 r = Matrix4::identity();
				r.m_[12] = t.x_; r.m_[13] = t.y_; r.m_[14] = t.z_;
				return r;
			}

			static Matrix4 scale(const Vector3& s) {
				Matrix4 r = Matrix4::identity();
				r.m_[0] = s.x_; r.m_[5] = s.y_; r.m_[10] = s.z_;
				return r;
			}

			static Matrix4 fromQuaternion(const Quaternion& quaternion) {
				Quaternion q = quaternion.normalized();

				Matrix4 r = Matrix4::identity();

				float x = q.x_;
				float y = q.y_;
				float z = q.z_;
				float w = q.w_;

				r.m_[0] = 1.0f - 2.0f * (y * y + z * z);
				r.m_[4] = 2.0f * (x * y - w * z);
				r.m_[8] = 2.0f * (x * z + w * y);

				r.m_[1] = 2.0f * (x * y + w * z);
				r.m_[5] = 1.0f - 2.0f * (x * x + z * z);
				r.m_[9] = 2.0f * (y * z - w * x);

				r.m_[2] = 2.0f * (x * z - w * y);
				r.m_[6] = 2.0f * (y * z + w * x);
				r.m_[10] = 1.0f - 2.0f * (x * x + y * y);

				return r;
			}

			Matrix4 operator*(const Matrix4& b) const {
				Matrix4 r;
				for (int col = 0; col < 4; ++col) {
					for (int row = 0; row < 4; ++row) {
						float sum = 0;
						for (int k = 0; k < 4; ++k) {
							sum += m_[k * 4 + row] * b.m_[col * 4 + k];
						}
						r.m_[col * 4 + row] = sum;
					}
				}
				return r;
			}
	};

	class Transform {
		public:
			Vector3 position_ = { 0.0f, 0.0f, 0.0f };
			Quaternion rotation_ = Quaternion::identity();
			Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

			Transform() = default;

			Transform(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
				: 
				position_(position),
				rotation_(rotation.normalized()),
				scale_(scale) 
			{}

			Matrix4 toMatrix() const {
				return Matrix4::translation(position_) * Matrix4::fromQuaternion(rotation_) * Matrix4::scale(scale_);
			}

			// Compose (For hierarchical  transform)
			Transform composedWith(const Transform& parent) const {
				Transform result;

				result.position_ = parent.position_ + parent.rotation_.rotate(parent.scale_ * position_);
				result.rotation_ = (parent.rotation_ * rotation_).normalized();
				result.scale_ = parent.scale_ * scale_;

				return result;
			}

			// Rotate a transform around a pivot position
			Transform& rotateAround(const Vector3& pivot, const Quaternion& rotation) {
				Quaternion q = rotation.normalized();

				position_ = pivot + q.rotate(position_ - pivot);
				rotation_ = (q * rotation_).normalized();

				return *this;
			}

			// Rotate a transform around a pivot position
			Transform& rotateAround(const Vector3& pivot, const Vector3& axis, float radians) {
				return rotateAround(pivot, Quaternion::fromAxisAngle(axis, radians));
			}

			// Used for drawing, this makes a copy of a transform with the rotateAround operation instead of performing it
			Transform makeRotatedAround(const Vector3& pivot, const Quaternion& rotation) const {
				Transform result = *this;
				result.rotateAround(pivot, rotation);
				return result;
			}

			// Used for drawing, this makes a copy of a transform with the rotateAround operation instead of performing it
			Transform makeRotatedAround(const Vector3& pivot, const Vector3& axis, float radians) const {
				return makeRotatedAround(pivot, Quaternion::fromAxisAngle(axis, radians));
			}

			Vector3 forward() const { return rotation_.rotate({ 0.0f, 0.0f, -1.0f }); }
			Vector3 right() const { return rotation_.rotate({ 1.0f, 0.0f, 0.0f }); }
			Vector3 up() const { return rotation_.rotate({ 0.0f, 1.0f, 0.0f }); }
	};

}

#endif